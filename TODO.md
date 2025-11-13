# ✅ TODO - Mini Projet S.I.M.S.

## 📅 Planning Recommandé

---

## JOUR 1: Configuration et Découverte (30 min)

### ⏰ Matin ou Après-midi

#### Étape 1: Setup Wokwi (10 min)
- [ ] Créer compte Wokwi (si nécessaire)
- [ ] Ouvrir https://wokwi.com/projects/new/esp32
- [ ] Copier `sketch.ino` dans l'éditeur
- [ ] Copier `diagram.json` dans l'onglet diagram.json
- [ ] Créer `libraries.txt` et copier le contenu
- [ ] Sauvegarder le projet Wokwi

#### Étape 2: Premier Test (10 min)
- [ ] Cliquer sur "Start Simulation" ▶
- [ ] Ouvrir Serial Monitor 💬
- [ ] Observer le menu qui s'affiche
- [ ] Taper `STATUS` et voir le résultat
- [ ] Taper `MENU` pour voir toutes les commandes
- [ ] Observer le LCD qui change d'affichage

#### Étape 3: Lecture Documentation (10 min)
- [ ] Lire RESUME.md (5 min)
- [ ] Parcourir QUICK_START.md (5 min)
- [ ] Noter les questions éventuelles

**✅ Fin Jour 1: Vous savez utiliser le projet !**

---

## JOUR 2: Tests de Base (1h)

### ⏰ Session de 1 heure

#### Préparation (5 min)
- [ ] Relire GUIDE_WOKWI.md - Section "Tester les Alarmes"
- [ ] Ouvrir SCENARIOS_TEST.md
- [ ] Préparer un document pour noter les résultats

#### Test 1: Initialisation (5 min)
- [ ] Suivre SCENARIOS_TEST.md - Test 1
- [ ] Vérifier tous les messages de démarrage
- [ ] Cocher les validations
- [ ] Prendre capture d'écran: Vue d'ensemble

#### Test 2: Conditions Normales (5 min)
- [ ] Suivre SCENARIOS_TEST.md - Test 2
- [ ] Observer les valeurs affichées
- [ ] Vérifier qu'il n'y a pas d'alarme
- [ ] Prendre capture d'écran: LCD normal

#### Test 3: Alarme Température (10 min)
- [ ] Suivre SCENARIOS_TEST.md - Test 3
- [ ] Cliquer DHT22, mettre T° à 35°C
- [ ] Observer: Buzzer, LED rouge, Relais
- [ ] Noter le message sur LCD
- [ ] Prendre capture d'écran: État alarme
- [ ] Remettre T° à 24°C
- [ ] Vérifier désactivation alarme

#### Test 4: Alarme Gaz (10 min)
- [ ] Suivre SCENARIOS_TEST.md - Test 4
- [ ] Cliquer Joystick, déplacer vers le haut
- [ ] Observer les réactions
- [ ] Noter niveau gaz affiché
- [ ] Prendre capture d'écran
- [ ] Remettre joystick au centre

#### Test 5: Alarme Humidité (5 min)
- [ ] Suivre SCENARIOS_TEST.md - Test 5
- [ ] DHT22: Humidité à 85%
- [ ] Observer l'alarme
- [ ] Prendre capture

#### Test 6: Alarme Luminosité (5 min)
- [ ] Suivre SCENARIOS_TEST.md - Test 6
- [ ] LDR: Réduire luminosité à 10%
- [ ] Observer l'alarme
- [ ] Prendre capture

#### Bilan (5 min)
- [ ] Remplir tableau de résultats
- [ ] Noter observations/remarques
- [ ] Identifier problèmes éventuels

**✅ Fin Jour 2: Tests de base validés !**

---

## JOUR 3: Tests Avancés (1h30)

### ⏰ Session de 1h30

#### Test 7: Alarmes Multiples (15 min)
- [ ] Suivre SCENARIOS_TEST.md - Test 7
- [ ] DHT22: T° 35°C, Hum 85%
- [ ] Joystick: Position haute (gaz)
- [ ] LDR: Luminosité faible
- [ ] Observer le message combiné
- [ ] Tester désactivation progressive
- [ ] Prendre captures

#### Test 8: Configuration Serial (20 min)
- [ ] Suivre SCENARIOS_TEST.md - Test 8
- [ ] Tester: `MENU`
- [ ] Tester: `STATUS`
- [ ] Tester: `SETTEMP 40`
- [ ] Vérifier changement de seuil
- [ ] Tester: `SETHUM 75`
- [ ] Tester: `SETGAS 1500`
- [ ] Tester: `SETLIGHT 800`
- [ ] Tester: `RESET`
- [ ] Tester commandes invalides
- [ ] Prendre capture: Menu complet
- [ ] Prendre capture: Configuration en action

#### Test 9: EEPROM (15 min) ⭐ IMPORTANT
- [ ] Suivre SCENARIOS_TEST.md - Test 9
- [ ] Configurer: `SETTEMP 28`
- [ ] Configurer: `SETHUM 65`
- [ ] Configurer: `SETGAS 1800`
- [ ] Configurer: `SETLIGHT 600`
- [ ] Noter les messages de sauvegarde
- [ ] Taper `STATUS` et noter les valeurs
- [ ] **Arrêter la simulation** (bouton rouge)
- [ ] **Redémarrer la simulation**
- [ ] Observer message "[EEPROM] Seuils chargés"
- [ ] Taper `STATUS` à nouveau
- [ ] **Vérifier que les seuils sont identiques**
- [ ] Prendre capture AVANT redémarrage
- [ ] Prendre capture APRÈS redémarrage
- [ ] ✅ PREUVE DE PERSISTANCE

#### Test 10: Affichage LCD Rotatif (10 min)
- [ ] Suivre SCENARIOS_TEST.md - Test 10
- [ ] Observer pendant 15 secondes
- [ ] Noter les 3 écrans différents
- [ ] Vérifier rotation toutes les 5s

#### Test 11: LED de Statut (5 min)
- [ ] Suivre SCENARIOS_TEST.md - Test 11
- [ ] Observer LED bleue pendant 10s
- [ ] Compter les clignotements
- [ ] Vérifier: 1 Hz (1/seconde)

#### Test 12: ThingSpeak (Optionnel) (15 min)
- [ ] Créer compte ThingSpeak
- [ ] Créer un Channel
- [ ] Configurer 5 Fields
- [ ] Copier API Key
- [ ] Modifier le code: ligne `thingSpeakApiKey`
- [ ] Redémarrer simulation
- [ ] Attendre 20 secondes
- [ ] Vérifier données sur ThingSpeak
- [ ] Prendre capture: Dashboard

#### Bilan (10 min)
- [ ] Remplir tableau final des tests
- [ ] Compter: Tests réussis / Total
- [ ] Noter difficultés rencontrées
- [ ] Noter solutions trouvées

**✅ Fin Jour 3: Tous les tests validés ! 🎉**

---

## JOUR 4: Rédaction du Rapport (2-3h)

### ⏰ Session de 2-3 heures

#### Préparation (15 min)
- [ ] Ouvrir RAPPORT_EXEMPLE.md
- [ ] Lire la structure complète
- [ ] Rassembler toutes les captures d'écran
- [ ] Rassembler notes de tests

#### Section 1: Introduction (30 min)
- [ ] Contexte du projet
- [ ] Objectifs principaux et secondaires
- [ ] Cahier des charges (requis + réalisé)

#### Section 2: Conception (30 min)
- [ ] Analyse fonctionnelle
- [ ] Liste des composants (tableau)
- [ ] Schéma de connexion
- [ ] Architecture logicielle
- [ ] Justification des choix

#### Section 3: Réalisation (30 min)
- [ ] Implémentation du code
- [ ] Gestion des capteurs
- [ ] Gestion des alarmes
- [ ] Persistance EEPROM
- [ ] Interface Serial
- [ ] Optimisations réalisées

#### Section 4: Tests et Validation (30 min)
- [ ] Méthodologie de test
- [ ] Tableau de résultats (12 tests)
- [ ] Captures d'écran annotées
- [ ] Problèmes rencontrés et solutions
- [ ] Validation finale

#### Section 5: Extensions (15 min)
- [ ] EEPROM: Description et avantages
- [ ] Menu interactif: Fonctionnalités
- [ ] IoT ThingSpeak: Architecture
- [ ] Affichage rotatif
- [ ] Relais intelligent
- [ ] Améliorations futures possibles

#### Section 6: Conclusion (15 min)
- [ ] Objectifs atteints (100%)
- [ ] Compétences acquises
- [ ] Points forts du projet
- [ ] Difficultés et solutions
- [ ] Apports pédagogiques
- [ ] Perspectives d'évolution

#### Section 7: Annexes (15 min)
- [ ] Référence au code source (sketch.ino)
- [ ] Référence au schéma (diagram.json)
- [ ] Liste des bibliothèques
- [ ] Références et liens
- [ ] Glossaire des termes techniques

#### Relecture et Finition (20 min)
- [ ] Relire tout le rapport
- [ ] Vérifier orthographe
- [ ] Vérifier numérotation
- [ ] Vérifier que toutes les captures sont présentes
- [ ] Vérifier mise en forme
- [ ] Exporter en PDF

**✅ Fin Jour 4: Rapport complet et professionnel ! 📄**

---

## JOUR 5: Préparation Présentation (1h)

### ⏰ Session de 1 heure

#### Préparation Contenu (20 min)
- [ ] Lire GUIDE_PRESENTATION.md complètement
- [ ] Imprimer le plan de présentation
- [ ] Préparer les captures d'écran à montrer
- [ ] Ouvrir le projet Wokwi dans un onglet
- [ ] Ouvrir le Serial Monitor
- [ ] Zoomer sur Wokwi pour bonne visibilité

#### Répétition 1 (15 min)
- [ ] Suivre le plan minute par minute
- [ ] Se chronométrer
- [ ] Noter les parties trop longues
- [ ] Noter ce qui manque de clarté

#### Répétition 2 (15 min)
- [ ] Corriger les points faibles
- [ ] Se chronométrer à nouveau
- [ ] Vérifier fluidité
- [ ] Pratiquer les transitions

#### Répétition 3 (10 min)
- [ ] Présentation finale complète
- [ ] Sans regarder les notes
- [ ] Temps cible: 7-10 minutes
- [ ] ✅ Validation

#### Préparation Questions (10 min)
- [ ] Lire section "Réponses aux Questions Fréquentes"
- [ ] Préparer réponses personnelles
- [ ] Anticiper questions spécifiques

#### Checklist Finale (10 min)
- [ ] Wokwi fonctionne
- [ ] Connexion internet OK
- [ ] Captures accessibles
- [ ] Plan imprimé
- [ ] Code source accessible
- [ ] Rapport finalisé
- [ ] État d'esprit positif ✅

**✅ Fin Jour 5: Prêt pour la présentation ! 🎤**

---

## JOUR J: Présentation (10-15 min)

### ⏰ Le grand jour !

#### Avant la Présentation (15 min avant)
- [ ] Arriver en avance
- [ ] Tester connexion internet
- [ ] Ouvrir Wokwi
- [ ] Ouvrir Serial Monitor
- [ ] Zoomer pour visibilité
- [ ] Préparer captures d'écran
- [ ] Relire plan une dernière fois
- [ ] Respirer profondément 😊

#### Pendant la Présentation (7-10 min)
- [ ] Commencer par phrase d'accroche
- [ ] Suivre le plan préparé
- [ ] Montrer ET expliquer en même temps
- [ ] Faire les 3 démos clés:
  - [ ] Alarme température
  - [ ] Configuration Serial
  - [ ] Persistance EEPROM
- [ ] Rester calme et souriant
- [ ] Regarder l'audience
- [ ] Conclure de manière positive

#### Questions-Réponses (5 min)
- [ ] Écouter attentivement les questions
- [ ] Prendre 2 secondes pour réfléchir
- [ ] Répondre clairement et brièvement
- [ ] Montrer sur Wokwi si nécessaire
- [ ] Rester positif même si on ne sait pas

#### Après la Présentation
- [ ] Remercier les évaluateurs
- [ ] Noter les retours
- [ ] Célébrer ! 🎉

**✅ PROJET TERMINÉ ! BRAVO ! 🎊**

---

## 📊 Suivi de Progression

### Cocher au fur et à mesure

#### Configuration (Jour 1)
- [ ] Projet créé sur Wokwi
- [ ] Première simulation réussie
- [ ] Documentation lue

#### Tests (Jours 2-3)
- [ ] 6 tests de base réalisés
- [ ] 6 tests avancés réalisés
- [ ] Captures d'écran prises
- [ ] Résultats notés

#### Rapport (Jour 4)
- [ ] Rapport commencé
- [ ] Toutes les sections rédigées
- [ ] Rapport relu
- [ ] PDF exporté

#### Présentation (Jour 5)
- [ ] Guide lu
- [ ] 3 répétitions effectuées
- [ ] Questions anticipées
- [ ] Checklist validée

#### Réussite (Jour J)
- [ ] Présentation réalisée
- [ ] Questions répondues
- [ ] Projet validé ! ✅

---

## 📌 Notes Importantes

### ⚠️ À NE PAS OUBLIER

1. **EEPROM = Extension Clé** ⭐
   - C'est l'extension la plus impressionnante
   - ABSOLUMENT la montrer en présentation
   - Prendre 2 captures: avant/après redémarrage

2. **Serial Monitor = Preuve de Fonctionnement**
   - Toujours ouvert pendant les démos
   - Montre les messages système
   - Prouve que tout fonctionne

3. **Captures d'Écran = Preuves**
   - Minimum 6 captures obligatoires
   - Les annoter si possible
   - Les intégrer dans le rapport

4. **Timing = Respect**
   - Ne pas dépasser 10 minutes
   - Laisser du temps pour les questions
   - S'entraîner avec chronomètre

### 💡 Astuces

- **Sauvegarder** régulièrement sur Wokwi
- **Documenter** chaque problème rencontré
- **Tester** avant chaque démonstration
- **Répéter** la présentation plusieurs fois
- **Rester calme** et confiant

---

## 🎯 Objectif Final

### Votre mission:

✅ Créer un système fonctionnel  
✅ Le tester exhaustivement  
✅ Le documenter professionnellement  
✅ Le présenter avec succès  

### Vous avez tout pour réussir:

- ✅ Un excellent projet
- ✅ Une documentation complète
- ✅ Des guides détaillés
- ✅ Des scénarios de test
- ✅ Un plan de présentation

---

## 🚀 Motivation

> "Le succès n'est pas final, l'échec n'est pas fatal:  
> c'est le courage de continuer qui compte."  
> — Winston Churchill

**Vous pouvez le faire ! 💪**

---

## ✅ Checklist Ultra-Rapide

- [ ] Setup Wokwi
- [ ] Tests de base (1-6)
- [ ] Tests avancés (7-12)
- [ ] Rapport rédigé
- [ ] Présentation préparée
- [ ] **PRÊT ! 🎉**

---

**Commencez maintenant et suivez ce plan étape par étape !**

**Bon courage ! Vous allez réussir ! 🌟**
