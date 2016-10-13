Redes de Computadores
1º Semestre - 2016/17
LEIC-A

RC Translate -
Grupo 14
81082 - Nuno Gonçalves
81205 - Alice Dourado
81500 - Rodrigo Rato

Geral - 
	Apesar de sabermos que a Makefile, segundo o enunciado, deveria colocar os executáveis na directoria raiz, optamos como sugerido pelo professor por manter cada aplicação na sua pasta respectiva para não haverem "colisões" dos ficheiros file_translate.txt e text_translate.txt (e ainda das imagens a ser transferidas). 

	O código fonte de cada aplicão está na sua directoria respectiva. Apesar do código do servidor TRS ser igual para os dois incluídos (fácilmente provado com o comando "diff"), o ficheiro fonte (trs_server.cpp) está incluído nas duas pastas para tornar a Makefile mais simples. 


Servidores TRS -
	Temos disponível dois servidores TRS, um para Inglês e outro para Francês, disponíveis respectivamente nas pastas "/trs_server_ingles/" e "/trs_server_frances/".


Makefiles -
	Tanto na directoria raiz como nas pastas de cada aplicação existe uma Makefile que compila os ficheiros necesários a cada aplicação.

	A Makefile na directoria raiz chama as que estão nas suas sub-directorias logo ao fazer make na primeira, todas as aplicações do projecto ficam compiladas nas respectivas pastas.
	


Lista de palavras disponíveis para traduzir -
	TRS #1 - Inglês:
		dog, cat, mouse, we, can, translate, sentences, table, goodbye, hello, world, coffee, word, image, news, map, file, open, networks, computer;

	TRS #2 - Francês:
		chien, chat, souris, nous, pouvez, traduire, phrases, table, pain, bonjour, monde, argent, mot, image, nouvelles, carte, fichier, ouvert, réseau, ordinateur;

Ficheiros disponíveis - 
	Quando o TRS recebe alguma imagem guarda-a com o nome original e o prefixo "totranslate_".
	Já o User, quando recebe uma imagem, guarda-a com o seu nome e o prefixo "translated_".

	Para enviar do User para o TRS #1 - Inglês:
		cat.jpg, dog.jpg, hedgehog.jpg, owl.jpg, sloth.jpg;

	Para enviar do User para o TRS #2 - Francês:
		chat.jpg, chien.jpg, herisson.jpg, chouette.jpg, paresseux.jpg;

	E os ficheiros resposta, respectivamente:
		gato.jpg, cao.jpg, ourico.jpg, coruja.jpg, preguica.jpg;