let ws;

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
            document.getElementById("feu_arriere").checked = p.feu_arriere;
            document.getElementById("lumiere_interieure").checked = p.lumiere_interieure;
            document.getElementById("servo_porte").checked = p.servo_porte;
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
    });
}

/* ---------------------------------------------------------------------------
 * DÉFINIR FX
 * ------------------------------------------------------------------------- */
function definirFx() {
    const fxArriere   = document.getElementById("feu_arriere").checked;
    const fxInterieur = document.getElementById("lumiere_interieure").checked;
    const fxServo     = document.getElementById("servo_porte").checked;

    const json = {
        fx: {
            feu_arriere: fxArriere,
            lumiere_interieure: fxInterieur,
            servo_porte: fxServo
        }
    };

    fetch("/definirFx", {
        method: "POST",
        headers: { "Content-Type": "application/json" },
        body: JSON.stringify(json)
    });
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
