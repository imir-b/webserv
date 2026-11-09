/**
 * ## TO-DO LIST ##
 *
 * 5. CONSTRUCTION DES RÉPONSES HTTP
 *     - Créer une classe HttpResponse
 *     - Générer les status codes (200, 404, 403, 500...)
 *     - Générer les headers (Content-Type, Content-Length...)
 *     - Générer les réponses personnalisées
 *     - Gérer Connection: keep-alive / close
 *
 * 6. GESTION DES ROUTES ET FICHIERS
 *     - Implémenter GET (servir HTML, CSS, JS, images...)
 *     - Implémenter POST (réception de données / uploads)
 *     - Implémenter DELETE (suppression de ressources)
 *     - Gérer les MIME types
 *     - Gérer les redirections HTTP
 *     - Gérer les pages d'erreur personnalisées
 *     - Sécuriser les chemins (éviter les directory traversals)
 *
 * 7. FICHIER DE CONFIGURATION
 *     - Parser un fichier de configuration
 *     - Gérer les ports et adresses d'écoute
 *     - Gérer les server names et les locations
 *     - Gérer les root directories et index files
 *     - Configurer les méthodes HTTP autorisées
 *     - Configurer les limites de taille des bodies
 *     - Gérer plusieurs serveurs virtuels
 *
 * 8. CGI (COMMON GATEWAY INTERFACE)
 *     - Comprendre fork(), execve(), pipe(), dup2()
 *     - Exécuter un script CGI
 *     - Transmettre les données de la requête au CGI
 *     - Récupérer stdout et les headers CGI
 *     - Gérer les erreurs et les timeouts CGI
 *
 * 9. TESTS ET ROBUSTESSE
 *     - Tester avec curl, nc et un navigateur
 *     - Tester plusieurs clients simultanés
 *     - Tester les requêtes fragmentées
 *     - Tester les gros fichiers et les uploads
 *     - Tester les clients lents et les déconnexions
 *     - Vérifier les fuites de FD et de mémoire
 *     - Comparer les comportements avec nginx
 *     - Vérifier la conformité C++98
 *
 */