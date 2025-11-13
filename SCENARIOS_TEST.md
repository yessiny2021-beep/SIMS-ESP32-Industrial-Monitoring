# 📋 SCÉNARIOS DE TEST - S.I.M.S.

## Vue d'ensemble
Ce document détaille les scénarios de test à réaliser pour valider le bon fonctionnement du système S.I.M.S.

---

## 🧪 TEST 1: Initialisation du Système

### Objectif
Vérifier que le système démarre correctement et initialise tous les composants.

### Procédure
1. Démarrer la simulation Wokwi
2. Observer le Serial Monitor
3. Observer l'écran LCD

### Résultats Attendus
- ✅ Message "S.I.M.S. v1.0" sur LCD
- ✅ Initialisation DHT22 confirmée dans Serial
- ✅ Connexion WiFi (tentative)
- ✅ Seuils chargés depuis EEPROM
- ✅ Menu affiché dans Serial Monitor
- ✅ Message "Système prêt!" sur LCD
- ✅ LED bleue commence à clignoter

### Validation
- [ ] Tous les messages apparaissent dans le bon ordre
- [ ] Pas d'erreur de compilation
- [ ] LCD fonctionne correctement
- [ ] LED bleue clignote à 1Hz

---

## 🌡️ TEST 2: Mesure en Conditions Normales

### Objectif
Vérifier que les capteurs fonctionnent et affichent des valeurs cohérentes.

### Conditions Initiales
- Température DHT22: 24°C (défaut)
- Humidité DHT22: 50% (défaut)
- Gaz (joystick): Position centrale (~2000)
- Luminosité (LDR): Valeur moyenne (~2000)

### Procédure
1. Laisser le système fonctionner 10 secondes
2. Observer l'affichage LCD qui alterne
3. Lire les valeurs dans Serial Monitor
4. Taper "STATUS" dans Serial Monitor

### Résultats Attendus
- ✅ Température affichée: ~24°C
- ✅ Humidité affichée: ~50%
- ✅ Niveau gaz: ~2000
- ✅ Luminosité: ~2000
- ✅ Pas d'alarme active
- ✅ LCD affiche "Systeme: OK"
- ✅ LED rouge éteinte
- ✅ Buzzer silencieux
- ✅ Relais désactivé

### Validation
- [ ] Toutes les valeurs sont cohérentes
- [ ] Pas d'alarme déclenchée
- [ ] Affichage LCD alterne tous les 5 secondes
- [ ] Commande STATUS fonctionne

---

## 🔥 TEST 3: Alarme Température Élevée

### Objectif
Vérifier le déclenchement de l'alarme en cas de température excessive.

### Procédure
1. Cliquer sur le capteur DHT22 dans Wokwi
2. Changer la température à 35°C
3. Cliquer sur "Update"
4. Observer pendant 5 secondes

### Résultats Attendus
- ✅ Message "!!! ALARME ACTIVÉE !!!" dans Serial
- ✅ Raison: "TEMP ELEVEE!"
- ✅ Buzzer émet un son (1000Hz)
- ✅ LED rouge allumée
- ✅ Relais activé (ventilateur ON)
- ✅ LCD affiche "!!! ALARME !!!"
- ✅ Message sur LCD: "TEMP ELEVEE!"

### Procédure de Retour
1. Remettre la température à 24°C
2. Observer la désactivation

### Résultats Désactivation
- ✅ Message "Conditions normales rétablies"
- ✅ Buzzer s'arrête
- ✅ LED rouge s'éteint
- ✅ Relais désactivé
- ✅ LCD affiche "Systeme: OK"

### Validation
- [ ] Alarme se déclenche immédiatement
- [ ] Tous les actionneurs réagissent
- [ ] Alarme se désactive au retour à la normale
- [ ] Pas de fausse alarme

---

## 💨 TEST 4: Détection de Gaz Dangereux

### Objectif
Vérifier la détection de gaz et le déclenchement de l'extracteur.

### Procédure
1. Cliquer sur le joystick (capteur MQ2 simulé)
2. Déplacer le stick vertical vers le haut au maximum
3. Observer les réactions

### Résultats Attendus
- ✅ Niveau gaz > 2000 (seuil par défaut)
- ✅ Message "GAZ DETECTE!"
- ✅ Alarme sonore activée
- ✅ LED rouge allumée
- ✅ Relais activé (extracteur ON)
- ✅ Affichage sur LCD

### Procédure de Test des Seuils
1. Taper "SETGAS 3000" dans Serial
2. Remettre joystick à zéro
3. Déplacer progressivement vers le haut
4. Observer le déclenchement à 3000

### Validation
- [ ] Détection de gaz fonctionnelle
- [ ] Seuil configurable via Serial
- [ ] Configuration sauvegardée en EEPROM
- [ ] Extracteur s'active automatiquement

---

## 💧 TEST 5: Humidité Excessive

### Objectif
Tester l'alarme d'humidité élevée.

### Procédure
1. Cliquer sur le capteur DHT22
2. Changer l'humidité à 85%
3. Cliquer "Update"

### Résultats Attendus
- ✅ Message "HUM ELEVEE!"
- ✅ Alarme activée
- ✅ Valeur affichée sur LCD: 85%

### Test de Seuil Personnalisé
1. Taper "SETHUM 90" dans Serial
2. Vérifier que l'alarme se désactive
3. Remettre humidité à 95%
4. Vérifier que l'alarme se réactive

### Validation
- [ ] Alarme humidité fonctionnelle
- [ ] Seuil personnalisable
- [ ] Affichage correct des valeurs

---

## 💡 TEST 6: Luminosité Insuffisante

### Objectif
Vérifier la détection de luminosité faible.

### Procédure
1. Cliquer sur le photorésisteur (LDR)
2. Réduire la luminosité à 10%
3. Observer la réaction

### Résultats Attendus
- ✅ Message "LUMIERE FAIBLE!"
- ✅ Alarme activée
- ✅ Valeur affichée sur LCD

### Test Seuil Luminosité
1. Taper "SETLIGHT 1000" dans Serial
2. Ajuster LDR pour tester le nouveau seuil

### Validation
- [ ] Détection luminosité fonctionnelle
- [ ] Seuil ajustable
- [ ] Réaction appropriée

---

## 🔄 TEST 7: Alarmes Multiples Simultanées

### Objectif
Tester le comportement avec plusieurs conditions d'alarme actives.

### Procédure
1. DHT22: Température → 35°C
2. DHT22: Humidité → 85%
3. Joystick: Position haute (gaz élevé)
4. LDR: Luminosité faible

### Résultats Attendus
- ✅ Message combiné: "TEMP ELEVEE! HUM ELEVEE! GAZ DETECTE! LUMIERE FAIBLE!"
- ✅ Alarme sonore continue
- ✅ LED rouge allumée
- ✅ Relais activé
- ✅ Toutes les valeurs visibles en alternance sur LCD

### Procédure de Désactivation Progressive
1. Normaliser la température
2. Observer: alarme reste active
3. Normaliser l'humidité
4. Observer: alarme reste active
5. Normaliser le gaz
6. Observer: alarme reste active
7. Normaliser la luminosité
8. Observer: alarme se désactive complètement

### Validation
- [ ] Système gère plusieurs alarmes simultanées
- [ ] Alarme reste active tant qu'une condition persiste
- [ ] Désactivation seulement quand tout est normal
- [ ] Affichage cohérent

---

## ⚙️ TEST 8: Configuration via Serial Monitor

### Objectif
Valider toutes les commandes de configuration.

### Test 8.1: Commande MENU
```
Input: MENU
Résultat attendu: ✅ Menu complet affiché
```

### Test 8.2: Commande STATUS
```
Input: STATUS
Résultat attendu: ✅ État complet du système
```

### Test 8.3: Configuration Température
```
Input: SETTEMP 40
Résultat attendu: ✅ "Seuil température mis à jour: 40.0°C"
```

### Test 8.4: Configuration Humidité
```
Input: SETHUM 75
Résultat attendu: ✅ "Seuil humidité mis à jour: 75.0%"
```

### Test 8.5: Configuration Gaz
```
Input: SETGAS 1500
Résultat attendu: ✅ "Seuil gaz mis à jour: 1500"
```

### Test 8.6: Configuration Luminosité
```
Input: SETLIGHT 800
Résultat attendu: ✅ "Seuil luminosité mis à jour: 800"
```

### Test 8.7: Commande RESET
```
Input: RESET
Résultat attendu: ✅ "Seuils réinitialisés aux valeurs par défaut"
```

### Test 8.8: Commandes Invalides
```
Input: SETTEMP -10
Résultat attendu: ✅ "Erreur: Valeur invalide"

Input: SETHUM 150
Résultat attendu: ✅ "Erreur: Valeur invalide"

Input: COMMANDEINCONNUE
Résultat attendu: ✅ "Commande inconnue"
```

### Validation
- [ ] Toutes les commandes fonctionnent
- [ ] Messages d'erreur appropriés
- [ ] Validation des entrées
- [ ] Menu clair et complet

---

## 💾 TEST 9: Persistance EEPROM

### Objectif
Vérifier que les configurations sont sauvegardées et rechargées.

### Procédure
1. Configuration initiale:
   ```
   SETTEMP 28
   SETHUM 65
   SETGAS 1800
   SETLIGHT 600
   ```

2. Vérifier les messages de sauvegarde

3. Arrêter la simulation (bouton rouge)

4. Redémarrer la simulation

5. Observer les messages d'initialisation

6. Taper "STATUS" pour vérifier les seuils

### Résultats Attendus
- ✅ Message "[EEPROM] Seuils sauvegardés!" après chaque SET
- ✅ Message "[EEPROM] Seuils chargés depuis la mémoire" au démarrage
- ✅ Seuils affichés correspondent aux valeurs configurées:
  - Température max: 28°C
  - Humidité max: 65%
  - Gaz max: 1800
  - Luminosité min: 600

### Test RESET EEPROM
1. Taper "RESET"
2. Redémarrer
3. Vérifier retour aux valeurs par défaut

### Validation
- [ ] Sauvegarde EEPROM fonctionnelle
- [ ] Chargement au démarrage correct
- [ ] RESET fonctionne
- [ ] Pas de perte de données

---

## 🔄 TEST 10: Affichage LCD Rotatif

### Objectif
Vérifier la rotation automatique des écrans LCD.

### Procédure
1. Laisser le système fonctionner 15 secondes
2. Observer les changements d'affichage

### Séquence Attendue (toutes les 5 secondes)
1. **Écran 1**: Température et Humidité
   ```
   Temp: 24.0°C
   Hum:  50.0%
   ```

2. **Écran 2**: Gaz et Luminosité
   ```
   Gaz:  2000
   Lum:  2000
   ```

3. **Écran 3**: État Système
   ```
   Systeme: OK
   WiFi: Connecte
   ```

4. Retour à l'Écran 1

### Validation
- [ ] Rotation automatique fonctionne
- [ ] Intervalle de 5 secondes respecté
- [ ] Affichage clair et lisible
- [ ] Pas de scintillement

---

## 🔵 TEST 11: LED de Statut

### Objectif
Vérifier le clignotement de la LED bleue d'état.

### Procédure
1. Observer la LED bleue pendant 10 secondes
2. Compter les clignotements

### Résultats Attendus
- ✅ LED clignote régulièrement
- ✅ Fréquence: 1 fois par seconde (1Hz)
- ✅ Durée ON = Durée OFF = 500ms
- ✅ Continue de clignoter même en alarme

### Validation
- [ ] Clignotement régulier
- [ ] Fréquence correcte
- [ ] Fonctionne en toutes circonstances

---

## 📡 TEST 12: Envoi ThingSpeak (Optionnel)

### Objectif
Tester l'envoi de données vers ThingSpeak.

### Prérequis
- Avoir configuré une clé API ThingSpeak valide dans le code

### Procédure
1. Modifier la ligne dans le code:
   ```cpp
   String thingSpeakApiKey = "VOTRE_CLE_API";
   ```

2. Redémarrer la simulation

3. Observer le Serial Monitor pour les messages ThingSpeak

4. Attendre 20 secondes

5. Vérifier sur le dashboard ThingSpeak

### Résultats Attendus
- ✅ Message "[ThingSpeak] Données envoyées avec succès!"
- ✅ Données visibles sur ThingSpeak:
  - Field 1: Température
  - Field 2: Humidité
  - Field 3: Niveau gaz
  - Field 4: Luminosité
  - Field 5: État alarme (0 ou 1)

### Validation
- [ ] Connexion WiFi établie
- [ ] Envoi toutes les 20 secondes
- [ ] Données correctes sur ThingSpeak
- [ ] Pas d'erreur HTTP

---

## 📊 RÉSUMÉ DES TESTS

### Checklist Complète

#### Fonctionnalités de Base
- [ ] TEST 1: Initialisation
- [ ] TEST 2: Mesures normales
- [ ] TEST 3: Alarme température
- [ ] TEST 4: Alarme gaz
- [ ] TEST 5: Alarme humidité
- [ ] TEST 6: Alarme luminosité

#### Fonctionnalités Avancées
- [ ] TEST 7: Alarmes multiples
- [ ] TEST 8: Configuration Serial
- [ ] TEST 9: Persistance EEPROM
- [ ] TEST 10: Affichage rotatif
- [ ] TEST 11: LED de statut
- [ ] TEST 12: ThingSpeak (optionnel)

---

## 📝 Rapport de Test

### Tableau de Résultats

| Test | Description | Statut | Remarques |
|------|-------------|--------|-----------|
| 1 | Initialisation | ⬜ | |
| 2 | Mesures normales | ⬜ | |
| 3 | Alarme température | ⬜ | |
| 4 | Alarme gaz | ⬜ | |
| 5 | Alarme humidité | ⬜ | |
| 6 | Alarme luminosité | ⬜ | |
| 7 | Alarmes multiples | ⬜ | |
| 8 | Config Serial | ⬜ | |
| 9 | EEPROM | ⬜ | |
| 10 | LCD rotatif | ⬜ | |
| 11 | LED statut | ⬜ | |
| 12 | ThingSpeak | ⬜ | |

**Légende**: ⬜ Non testé | ✅ Réussi | ❌ Échoué | ⚠️ Partiel

---

## 🎯 Critères de Validation Globale

Pour que le projet soit considéré comme validé:

1. ✅ **Tous les tests de base (1-6) réussis**
2. ✅ **Au moins 3 tests avancés (7-12) réussis**
3. ✅ **Aucune erreur critique**
4. ✅ **Comportement cohérent et prévisible**
5. ✅ **Documentation complète et claire**

---

**Bonne chance pour vos tests! 🚀**
