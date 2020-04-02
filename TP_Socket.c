#include <sys/time.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <netdb.h>
#include <stdlib.h>

char * GetMsg(char * ip, int PORT) {
	int fd;
	struct sockaddr_in address;
	struct hostent *host;

	char buff[100];
	char *somme = malloc (sizeof (char) * 100);
	char * test="boom";
	int err, sz;

	fd=socket(AF_INET, SOCK_STREAM,0);
	
	host=gethostbyname(ip);

	bzero(&address, sizeof(address));
	bcopy(host->h_addr_list[0],&address.sin_addr,sizeof(address.sin_addr));
	address.sin_family=AF_INET;
	address.sin_port=htons(PORT);

	err = connect(fd, (struct sockaddr*)&address, sizeof(address));	

	sz=1;
	bzero(somme,sizeof(somme));
	

	if (err==0) {malloc (sizeof (char) * 100);
		//printf("connexion établie \n");	
		while (sz>0) {		
			sz = send(fd, test, sizeof(test), 0);
			if(sz>0) {
				//printf("le nombre de bytes envoyés %d\n", sz);
				bzero(buff,sizeof(buff));
				sz = recv(fd, buff, sizeof(buff), 0);
				if (sz>0) {
					//printf("le nombre de bytes reçus %d\n", sz); 
					//printf("le MSG est : %s\n", buff);
					strcat(somme,buff);
				}
			}
		}
		//printf("la position est : %s\n", somme);
	} else {
		printf("connexion non établie\n");
	}
	err = shutdown(fd, SHUT_RDWR);
	
	return somme;
}


int main(int argc, char *argv[]) {
	double mode;
	sscanf(argv[1],"%lf", &mode);
	
	if (mode==1) {
		//on travaille en mode serveur qui quelque soit le message recu il renvoit l'information demandée, l'information est affichée aussi
	
		double demande;
		sscanf(argv[2],"%lf", &demande);
		
		ssize_t sz;
		socklen_t remotelen;
		int fd, s;
		struct sockaddr_in address, remote;
		char msg[100];
		int err;
		
		fd=socket(AF_INET, SOCK_STREAM, 0);
		
		address.sin_family = AF_INET;
		address.sin_addr.s_addr = INADDR_ANY;
		address.sin_port = htons(8010);
		
		err=bind(fd, (struct sockaddr *)&address, sizeof(address));
		err=listen(fd,5);
		
		remotelen = sizeof(remote);
		bzero(&remote, sizeof(remote));
		
		char *valeur=malloc (sizeof (char) * 100);
		
		if (demande==1) {
			//pour la temperature moyenne
			
			double moy=0.0;
			double val1, val2, val3, val4, val5, val6;
			
			sscanf(GetMsg("192.168.24.100",8000),"%lf", &val1);
			sscanf(GetMsg("192.168.24.101",8000),"%lf", &val2);
			sscanf(GetMsg("192.168.24.102",8000),"%lf", &val3);
			sscanf(GetMsg("192.168.24.103",8000),"%lf", &val4);
			sscanf(GetMsg("192.168.24.104",8000),"%lf", &val5);
			sscanf(GetMsg("192.168.24.105",8000),"%lf", &val6);
			
			moy=(val1+val2+val3+val4+val5+val6)/6;
			
			gcvt(moy,4,valeur);
			
			printf("le temperature moyenne est : \"%s\"\n", valeur);
		}else if (demande==2) {
			//pour la luminosité moyenne
			
			double moy=0.0;
			double val1, val2, val3, val4, val5, val6;
			
			sscanf(GetMsg("192.168.24.100",8001),"%lf", &val1);
			sscanf(GetMsg("192.168.24.101",8001),"%lf", &val2);
			sscanf(GetMsg("192.168.24.102",8001),"%lf", &val3);
			sscanf(GetMsg("192.168.24.103",8001),"%lf", &val4);
			sscanf(GetMsg("192.168.24.104",8001),"%lf", &val5);
			sscanf(GetMsg("192.168.24.105",8001),"%lf", &val6);
			
			moy=(val1+val2+val3+val4+val5+val6)/6;
			
			gcvt(moy,4,valeur);
			
			printf("la luminosité moyenne est : \"%s\"\n", valeur);
		} else if (demande==3) {
			//pour l'état des portes
			
			double val1, val2;
			
			sscanf(GetMsg("192.168.24.103",8003),"%lf", &val1);
			sscanf(GetMsg("192.168.24.104",8003),"%lf", &val2);
			
			if (val1==1 && val2==1) {
				strcat(valeur,"la salle est fermée");
			} else {
				strcat(valeur,"la salle est ouverte");
			}
			printf("résultat : \"%s\"\n", valeur);
		} else if (demande==4) {
			//pour la position GPS
			
			double val1;
			
			sscanf(GetMsg("192.168.24.106",8004),"%lf", &val1);
			
			gcvt(val1,4,valeur);
			
			printf("la position GPS est : \"%s\"\n", valeur);
		}
		
		
		sz=1;
		
		while (1) {
			s=accept(fd, (struct sockaddr*)&remote, &remotelen);
			if (s!=-1) {
				printf("connexion établie \n");	
				while (sz>0) {	
					
					bzero(msg,sizeof(msg));
					sz = recv(s, msg, sizeof(msg), 0);
					printf("le message est : \"%s\"\n", msg);
					if(sz>0) {
						msg[sz] = '\0';
						//strcat(somme,msg);
						strcat(valeur,"test test test test");
						sz = send(s, valeur, strlen(valeur), 0);
					}
				}
				//printf("le message est : \"%s\"\n", somme);
			}
			err = shutdown(s, SHUT_RDWR);
		} 
	} else if (mode==2) {
		//dans ce cas on travaille en mode client 
		
		int fd;
		struct sockaddr_in address;
		struct hostent *host;

		char buff[100];
		char *test="2";
		char somme[100];
		int err, sz;
		
		int PORT=8010;
		//port conventionnel entre nous deux

		fd=socket(AF_INET, SOCK_STREAM,0);
		
		host=gethostbyname("192.168.0.13");
		//adresse ip de mon voisin

		bzero(&address, sizeof(address));
		bcopy(host->h_addr_list[0],&address.sin_addr,sizeof(address.sin_addr));
		address.sin_family=AF_INET;
		address.sin_port=htons(PORT);

		err = connect(fd, (struct sockaddr*)&address, sizeof(address));	

		sz=1;
		bzero(somme,sizeof(somme));
		//somme est valeur ou je mets le message recu
		
		//1270x

		if (err==0) {
			printf("connexion établie \n");	
			while (sz>0) {		
				sz = send(fd, test, sizeof(test), 0);
				if(sz>0) {
					printf("le nombre de bytes envoyés %d\n", sz);
					bzero(buff,sizeof(buff));
					sz = recv(fd, buff, sizeof(buff), 0);
					if (sz>0) {
						printf("le nombre de bytes reçus %d\n", sz); 
						printf("le MSG est : %s\n", buff);
						strcat(somme,buff);
					}
				}
			}
			printf("le message complet est : %s\n", somme);
		} else {
			printf("connexion non établie\n");
		}
		err = shutdown(fd, SHUT_RDWR);
	}
}
