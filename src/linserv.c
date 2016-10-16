#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

logeintrag()
{
	time_t t = time(NULL);
	struct tm time = *localtime(&t);
	FILE *zeitupdate;
	zeitupdate = fopen("/home/ubuntu-usr/verbind.log", "a");
	fprintf(zeitupdate, "neue verb. am: %d-%d-%d, %d:%d:%d,", time.tm_year + 1900, time.tm_mon + 1, time.tm_mday, time.tm_hour, time.tm_min, time.tm_sec);
	return 1;
}

int main(int argc, char *argv[])

{
	/* socketfd und newsocketfd speichern filedescriptor von sockets
	portnr speichert portnr des ports auf dem das programm wartet
	clientaddrlen speichert die länge der clientaddresse */

	int socketfd;
	int newsocketfd;
	int portnr;
	int clientaddrlen;
	int n;


	char stringbuffer[256];
	char spielername[64];
	

	/* structs die in <netinet/in.h> definiert sind, die variablen serveraddr und
	clientaddr sind von mir gewählt, hier werden verschiedene Werte gespeichert */
	struct sockaddr_in serveraddr;
	struct sockaddr_in clientaddr;
	void *pointer = &serveraddr;

	/* kleiner Test ob alle variablen angegeben wurden */
	if (argc < 2)
	{
		fprintf(stderr, "Syntax: <Programmname> <Port Nr>");
		exit(1);
	};


	/*  Die socket() funktion gibt einen eintrag in die filedescriptortabelle zurück. 
	Die nummer die socket() zurückgibt wird in socketfd (socket file descriptor) 
	gespeichert. Ab jetzt wird diese nummer immer verwendet um vom socket zu lesen oder
	zu schreiben.
	(vgl. fd's die immer vorhanden sind: 0 für stdinput, 1 für stdoutput, 2 für stderror)
	socket() nimmt 3 Argumente: 
	1. Das erste ist die Address Family, wo wir die Internet Family benutzen, weil sie 
	die grösste Kompatibilität bietet. 
	
	2. Als zweites geben wir die Art des Sockets an. Stream Sockets behandeln verbindungen
	als einen fortlaufenden Datenstrom.
	
	3.Danach geben wir das kommunikationsprotokoll an, mit einer 0 überlassen wir die Wahl
	dem Betriebssystem. Bei einem Stream Socket wird es wahrscheinlich TCP wählen. Mit 
	
	wenn socket() -1 zurückgibt, gab es einen fehler beim erstellen des Sockets, dafür das
	kurze if() */

	socketfd = socket(AF_INET, SOCK_STREAM, 0);

	if (socketfd < 0)
	{
		fprintf(stderr, "Fehler beim erstellen des Sockets");
	};

	/* serveraddr mit nullen überschreiben, memset teil von <string.h> */
	memset(pointer,0,sizeof(serveraddr));

	/* die eingegebene portnr speichern, atoi() wandelt die als string eingegebene portnr 
	in eine typ int um, teil von <stdlib.h> */
	portnr = atoi(argv[1]);

	/*die folgenden 3 Variablen sind in netinet/in.h deklariert und werden jetzt definiert*/
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_port = htons(portnr); /*Umwandlung von little Endian auf Big Endian*/
	serveraddr.sin_addr.s_addr = INADDR_ANY; /*hier wird der socket an alle verfügbaren
	interfaces gebunden*/

	/* bind() bindet den socket an die pc addresse. Das mittlere Argument gibt die Addresse
	an, an die der socket gebunden werden soll */
	if (bind(socketfd, (struct sockaddr *) &serveraddr, sizeof(serveraddr)) < 0)
	{
		fprintf(stderr, "Fehler beim binden an Addresse");
	};

	/* listen() erlaubt es dem Programm auf dem socket auf eintreffende Verbindungen zu
	warten, das zweite Argument gibt an wie viele verbindungen in der "warteschlange"
	sein dürfen */
	listen(socketfd, 5);
	
	/* Wir brauchen die länge von der addresse des Clients für accept(). accept() braucht
	noch die addresse des PC's der sich mit diesem verbindet (clientaddr).
	accept() weckt den Prozess auf wenn ein client sich verbindet und gibt einen file
	decriptor zurück, für diese Verbindung wird der neue fd verwendet. */
	clientaddrlen = sizeof(clientaddrlen);
	newsocketfd = accept(socketfd, (struct sockaddr *) &clientaddr, &clientaddrlen);
	if (newsocketfd < 0)
	{
		fprintf(stderr, "Fehler bei eingehender Verbindung");
	};
	
	logeintrag();
	
	memset(stringbuffer,0,sizeof(stringbuffer));

	n = write(newsocketfd, "Willkommen, gib deinen Spielernamen ein: ", 42);
	if (n < 0)
	{
		fprintf(stderr, "Fehler beim senden der Daten");
	};
	
	n = read(newsocketfd, stringbuffer, 128);
	if (n < 0)
	{
		fprintf(stderr, "Fehler beim empfangen der Daten");
	};

	n = write(newsocketfd, "Datenaustausch erfolgreich", 27);
	if (n < 0)
	{
		fprintf(stderr, "Fehler beim senden der Daten");
	};
	strcpy(spielername, stringbuffer);

	FILE *filepointer;
	
	filepointer = fopen("/home/ubuntu-usr/spielername", "w");
	if (filepointer == 0)
	{
		fprintf(stderr, "Fehler beim Öffnen der Datei zum Schreiben");
	};

	fprintf(filepointer, spielername);

	fclose(filepointer);
	return 0;
}
