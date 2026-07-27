let ws;

/* ---------------------------------------------------------------------------
 * COLORATION JSON (avec classes CSS)
 * ------------------------------------------------------------------------- */
function colorizeJson(obj) {
    let json = JSON.stringify(obj, null, 2);

    return json
        .replace(/"(.*?)":/g, (match, key) => {
            return `<span class="json-key">"${key}"</span>:`;
        })
        .replace(/: "([^"]*)"/g, (match, value) => {
            return `: <span class="json-string">"${value}"</span>`;
        })
        .replace(/: (\d+)/g, (match, num) => {
            return `: <span class="json-number">${num}</span>`;
        })
        .replace(/: (true|false)/g, (match, bool) => {
            return `: <span class="json-boolean">${bool}</span>`;
        })
        .replace(/: null/g, `: <span class="json-null">null</span>`);
}

/* ---------------------------------------------------------------------------
 * INITIALISATION
 * ------------------------------------------------------------------------- */
window.onload = () => {
    chargerParametres();
    initWebSocket();
};

/* ---------------------------------------------------------------------------
 * CHARGER PARAMÈTRES
 * ------------------------------------------------------------------------- */
function chargerParametres() {
    fetch("/obtenirParametres")
        .then(r => r.json())
        .then(p => {

            document.getElementById("adresse").value = p.adresse;
            document.getElementById("wifi_actif").checked = p.wifi_actif;

            // Paramètres servo
            document.getElementById("servo_porte_angle_ouvert").value = p.servo_porte_angle_ouvert;
            document.getElementById("servo_porte_angle_ferme").value = p.servo_porte_angle_ferme;
            document.getElementById("servo_porte_vitesse").value = p.servo_porte_vitesse;

            // FX
            document.getElementById("feu_arriere").checked = p.feu_arriere;
            document.getElementById("lumiere_interieure").checked = p.lumiere_interieure;
            document.getElementById("servo_porte").checked = p.servo_porte;

            // ESSIEUX
            if (p.essieux) {
                // true → 4 essieux
                document.getElementById("essieux4").checked = true;
                document.getElementById("essieux2").checked = false;
            } else {
                // false → 2 essieux
                document.getElementById("essieux4").checked = false;
                document.getElementById("essieux2").checked = true;
            }

            // DEBUG
            document.getElementById("jsonDebug").innerHTML =
                colorizeJson(p);
        });
}

/* ---------------------------------------------------------------------------
 * ERREURS ADRESSE
 * ------------------------------------------------------------------------- */
function afficherErreur(msg) {
    const zone = document.getElementById("msgErreurAdresse");
    if (!zone) return;
    zone.innerText = msg;
    zone.style.display = "block";
    zone.style.color = "#ff5555";
}

function effacerErreur() {
    const zone = document.getElementById("msgErreurAdresse");
    if (!zone) return;
    zone.style.display = "none";
}

/* ---------------------------------------------------------------------------
 * VALIDATION ADRESSE
 * ------------------------------------------------------------------------- */
function validerAdresse(adresse) {
    const num = parseInt(adresse, 10);

    if (isNaN(num)) {
        afficherErreur("Adresse invalide : ce n'est pas un nombre.");
        return false;
    }

    if (num < 4001 || num > 4999) {
        afficherErreur("Adresse invalide : doit être comprise entre 4001 et 4999.");
        return false;
    }

    return true;
}

/* ---------------------------------------------------------------------------
 * DÉFINIR ADRESSE
 * ------------------------------------------------------------------------- */
function definirAdresse() {
    const adresseStr = document.getElementById("adresse").value;

    effacerErreur();

    if (!validerAdresse(adresseStr)) return;

    const adresse = parseInt(adresseStr, 10);

    fetch("/definirAdresse", {
        method: "POST",
        body: JSON.stringify({ adresse })
    })
        .then(r => {
            if (!r.ok) {
                return r.text().then(msg => afficherErreur(msg));
            }
            chargerParametres();
        });
}

/* ---------------------------------------------------------------------------
 * DÉFINIR WIFI
 * ------------------------------------------------------------------------- */
function definirWifi() {
    const wifi_actif = document.getElementById("wifi_actif").checked;

    fetch("/definirWifi", {
        method: "POST",
        body: JSON.stringify({ wifi_actif })
    })
        .then(() => chargerParametres());
}

/* ---------------------------------------------------------------------------
 * DÉFINIR FX
 * ------------------------------------------------------------------------- */
function definirFx() {

    const feu_arriere = document.getElementById("feu_arriere").checked;
    const lumiere_interieure = document.getElementById("lumiere_interieure").checked;
    const servo_porte = document.getElementById("servo_porte").checked;

    fetch("/definirFx", {
        method: "POST",
        headers: { "Content-Type": "application/json" },
        body: JSON.stringify({
            feu_arriere,
            lumiere_interieure,
            servo_porte
        })
    })
        .then(r => {
            if (!r.ok) {
                return r.text().then(msg => alert("Erreur FX : " + msg));
            }
            chargerParametres();
        })
        .catch(err => alert("Erreur réseau FX : " + err));
}

/* ---------------------------------------------------------------------------
 * SAUVEGARDE SERVO
 * ------------------------------------------------------------------------- */
function sauvegardeServo() {

    const servo_porte_angle_ouvert = parseInt(document.getElementById("servo_porte_angle_ouvert").value);
    const servo_porte_angle_ferme = parseInt(document.getElementById("servo_porte_angle_ferme").value);
    const servo_porte_vitesse = parseInt(document.getElementById("servo_porte_vitesse").value);

    fetch("/definirServoPorteParametres", {
        method: "POST",
        body: JSON.stringify({
            servo_porte_angle_ouvert,
            servo_porte_angle_ferme,
            servo_porte_vitesse
        })
    })
        .then(r => {
            if (!r.ok) {
                return r.text().then(msg => alert("Erreur servo : " + msg));
            }
            chargerParametres();
        })
}

/* ---------------------------------------------------------------------------
 * DEFINIR ESSIEUX
 * ------------------------------------------------------------------------- */
function choisirEssieux(val) {
    if (val === 2) {
        document.getElementById("essieux2").checked = true;
        document.getElementById("essieux4").checked = false;
    } else {
        document.getElementById("essieux2").checked = false;
        document.getElementById("essieux4").checked = true;
    }
}

function definirEssieux() {
    const essieux = document.getElementById("essieux4").checked;
    // false = 2 essieux
    // true  = 4 essieux

    fetch("/definirEssieux", {
        method: "POST",
        body: JSON.stringify({ essieux })
    })
        .then(r => {
            if (!r.ok) {
                return r.text().then(msg => alert("Erreur essieux : " + msg));
            }
            chargerParametres();
        })
}

/* ---------------------------------------------------------------------------
 * WEBSOCKET RAILCOM
 * ------------------------------------------------------------------------- */
function initWebSocket() {
    ws = new WebSocket("ws://" + window.location.host + "/ws");

    ws.onmessage = (event) => {
        document.getElementById("railcomLive").textContent = event.data;
    };

    ws.onopen = () => console.log("WebSocket connecté");
    ws.onclose = () => console.log("WebSocket déconnecté");
}

/* ---------------------------------------------------------------------------
 * REMISE A ZERO
 * ------------------------------------------------------------------------- */
function resetParametres() {
    if (!confirm("Confirmer la réinitialisation des paramètres ?")) return;

    fetch("/resetParametres", {
        method: "POST"
    })
        .then(r => {
            if (!r.ok) {
                return r.text().then(msg => alert("Erreur reset : " + msg));
            }
            chargerParametres();
        })
        .catch(err => {
            alert("Erreur réseau lors du reset : " + err);
        });
}

/* ---------------------------------------------------------------------------
 * SAUVEGARDE GENERALE — version simple (3 FX fixes)
 * ------------------------------------------------------------------------- */
function sauvegardeGenerale() {

    const adresse = parseInt(document.getElementById("adresse").value);
    const wifi_actif = document.getElementById("wifi_actif").checked;

    // Paramètres servo
    const servo_porte_angle_ouvert = parseInt(document.getElementById("servo_porte_angle_ouvert").value);
    const servo_porte_angle_ferme = parseInt(document.getElementById("servo_porte_angle_ferme").value);
    const servo_porte_vitesse = parseInt(document.getElementById("servo_porte_vitesse").value);

    // FX
    const feu_arriere = document.getElementById("feu_arriere").checked;
    const lumiere_interieure = document.getElementById("lumiere_interieure").checked;
    const servo_porte = document.getElementById("servo_porte").checked;

    // ESSIEUX
    const essieux = document.getElementById("essieux4").checked;
    // false = 2 essieux
    // true  = 4 essieux

    fetch("/sauvegardeGenerale", {
        method: "POST",
        headers: { "Content-Type": "application/json" },
        body: JSON.stringify({
            adresse,
            wifi_actif,
            servo_porte_angle_ouvert,
            servo_porte_angle_ferme,
            servo_porte_vitesse,
            feu_arriere,
            lumiere_interieure,
            servo_porte,
            essieux
        })
    })
        .then(r => {
            if (!r.ok) {
                return r.text().then(msg => alert("Erreur sauvegarde : " + msg));
            }
            alert("Sauvegarde effectuée !");
            chargerParametres();
        })
        .catch(err => alert("Erreur réseau : " + err));
}
