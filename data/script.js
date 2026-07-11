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

            // ⭐ Génération dynamique des FX
            const fxContainer = document.getElementById("fxContainer");
            fxContainer.innerHTML = "";

            for (const key in p.fx) {
                fxContainer.innerHTML += `
                    <div class="form-grid-2col">
                        <label>${key.replace(/_/g, " ")} :</label>
                        <input id="${key}" type="checkbox" class="fx-checkbox">
                    </div>
                `;
            }

            // ⭐ Mise à jour des valeurs
            for (const key in p.fx) {
                const el = document.getElementById(key);
                if (el) el.checked = p.fx[key];
            }

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
    const fx = {};

    // ⭐ FX dynamiques
    document.querySelectorAll(".fx-checkbox").forEach(el => {
        fx[el.id] = el.checked;
    });

    fetch("/definirFx", {
        method: "POST",
        headers: { "Content-Type": "application/json" },
        body: JSON.stringify({ fx })
    })
    .then(() => chargerParametres());
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
        // On recharge les paramètres après reset
        chargerParametres();
    })
    .catch(err => {
        alert("Erreur réseau lors du reset : " + err);
    });
}

/* ---------------------------------------------------------------------------
 * SAUVEGARDE GENERALE
 * ------------------------------------------------------------------------- */
function sauvegardeGenerale() {

    const adresse = parseInt(document.getElementById("adresse").value);
    const wifi_actif = document.getElementById("wifi_actif").checked;

    const fx = {};
    document.querySelectorAll(".fx-checkbox").forEach(el => {
        fx[el.id] = el.checked;
    });

    fetch("/sauvegardeGenerale", {
        method: "POST",
        headers: { "Content-Type": "application/json" },
        body: JSON.stringify({
            adresse,
            wifi_actif,
            fx
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
