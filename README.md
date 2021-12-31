**Pourcentage de la part de travail :**

Delacroix Grégoire : ...%

Avot Kévin : ...%

**Fonctionnalités implémentées :**

    - Séquencement : ";", "&&", "||".
    - Wildcards et jokers.
    - Changement de répertoire avec cd avec ~ pris en charge et répertoire optionnel.
    - Ctrl-C qui demande confirmation avant de quitter, et tue tous les processus.
    - Ctrl-C qui, lors d'une exécution en foreground, est propagée au processus en cours d'exécution.
    - status pour les codes de retours du dernier processus en foreground.
    - Exécution possible en background avec le symbole "&". Numéro de job affiché et réinitialisé quand plus aucun job.
    - Informations du processus en bakcground quand il se termine.
    - myjobs affiche la liste des processus en background.
    - Ctrl-Z pour stopper la commande en cours. Il devient un job.
    - Variables locales. set et unset.
    - Utilisation des variables locales avec "$". set pour afficher l'ensemble des variables locales.
    - Toutes les redirections avec chevrons sauf <.
    - Commande myls avec tous ses paramètres ainsi que les couleurs pour les différents types de fichier avec la prise en charge de multiples arguments/paramètres.
    - Gestion des pipelines mais avec une limite de un | par ligne.

**Fonctionnalités non implémentées :**

    - Commande fg et bg.
    - Variables d'environnement. set et setenv et donc mémoire partagée.
    - Commande myps
    - glob et pipe ne sont pas fonctionnels avec myls.

**Bugs :**

    - La commande set ne peut s'exécuter qu'en foreground, car elle fonctionne par le biais d'un pipe et le père attend une lecture dans cette dernière.
    - Ctrl-c tue les processus fils avant de demander à l'utilisateur. Le signal est propagé du processus père au fils.
