# Projet PRS

Built by :
Thibaut G.
Adrien C.

# Avancement

18/04/2017
1. on utilise pas correctement "select". à la fin de la handshake, on doit ajouter desc à set.
=> le select va voir si le desc est pret pour recevoir/envoyer à chaque passage dans la boucle principale.

#Useful links

using select properly : https://www.gnu.org/software/libc/manual/html_node/Waiting-for-I_002fO.html#Waiting-for-I_002fO
