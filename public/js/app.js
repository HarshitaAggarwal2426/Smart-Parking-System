document.addEventListener("DOMContentLoaded", function () {
// ===============================
// DASHBOARD AUTO LOAD
// ===============================
if (document.getElementById("total")) {

    fetch("/run", {
        method: "POST",
        headers: { "Content-Type": "application/json" },
        body: JSON.stringify({
            input: `9\n`
        })
    })
    .then(res => res.text())
    .then(data => {

        const totalMatch = data.match(/Total Vehicles:\s*(\d+)/);
        const freeMatch = data.match(/Available Slots:\s*(\d+)/);

        if (totalMatch)
            document.getElementById("total").innerText = totalMatch[1];

        if (freeMatch)
            document.getElementById("slots").innerText = freeMatch[1];
    })
    .catch(() => console.log("Dashboard load failed"));
}
// ===============================
// ADMIN LOGIN
// ===============================
document.getElementById("adminForm")?.addEventListener("submit", async function (e) {
    e.preventDefault();

    const password = document.getElementById("password").value.trim();
    const loader = document.getElementById("loader");

    try {
        loader.style.display = "flex";

        const response = await fetch("/run", {
            method: "POST",
            headers: { "Content-Type": "application/json" },
            body: JSON.stringify({
                input: `5\n${password}\n`
            })
        });

const data = await response.text();
console.log("ADMIN RESPONSE:", data);   // add this

if (!data || data.trim() === "") {
    showMessage("❌ No data received from server", "error");
    return;
}

convertOutputToSectionCards(data);

        // Extract revenue
        const revenueMatch = data.match(/Total Revenue:\s*Rs\s*(\d+)/);

        if (revenueMatch && document.getElementById("revenue")) {
            document.getElementById("revenue").innerText = revenueMatch[1];
        }

    } catch {
        showMessage("❌ Server Error", "error");
    } finally {
        loader.style.display = "none";
    }
});    
    // ===============================
    // PARK FORM
    // ===============================
    document.getElementById("parkForm")?.addEventListener("submit", async function (e) {
    e.preventDefault();

    const vehicle = document.getElementById("vehicle").value.trim();
    const phone = document.getElementById("phone").value.trim();
    const typeRadio = document.querySelector('input[name="type"]:checked');
    const loader = document.getElementById("loader");

    if (!vehicle || !phone || !typeRadio) {
        showMessage("⚠️ Please fill all fields", "error");
        return;
    }

    const type = typeRadio.value;

    try {
        if (loader) loader.style.display = "flex";

        const response = await fetch("/run", {
            method: "POST",
            headers: { "Content-Type": "application/json" },
            body: JSON.stringify({
                input: `1\n${vehicle}\n${type}\n${phone}\n`
            })
        });

        const data = await response.text();

        // SHOW IN OUTPUT BOX
        convertOutputToSectionCards(data);

    } catch {
        showMessage("❌ Server Error", "error");
    } finally {
        if (loader) loader.style.display = "none";
    }
});

    // ===============================
    // SHOW SLOTS BUTTON
    // ===============================
    document.getElementById("showSlots")?.addEventListener("click", async function () {

        const loader = document.getElementById("loader");

        try {
            loader.style.display = "flex";

            const response = await fetch("/run", {
                method: "POST",
                headers: { "Content-Type": "application/json" },
                body: JSON.stringify({
                    input: `3\n`
                })
            });

            const data = await response.text();
            renderSlotsFromText(data);

        } catch (err) {
            showMessage("❌ Failed to load slots", "error");
        } finally {
            loader.style.display = "none";
        }
    });
    
    document.getElementById("emergencyForm")?.addEventListener("submit", async function (e) {
    e.preventDefault();

    const form = this;
    const vehicle = form.querySelector('input[name="vehicle"]').value.trim();
    const phone = form.querySelector('input[name="phone"]').value.trim();
    const loader = document.getElementById("loader");

    if (!vehicle || !phone) {
        showMessage("⚠️ Fill all fields", "error");
        return;
    }

    try {
        loader.style.display = "flex";

        const response = await fetch("/run", {
            method: "POST",
            headers: { "Content-Type": "application/json" },
            body: JSON.stringify({
                input: `6\n${vehicle}\n${phone}\n`
            })
        });

        const data = await response.text();
        convertOutputToSectionCards(data);

    } catch {
        showMessage("❌ Server Error", "error");
    } finally {
        loader.style.display = "none";
    }
});

document.getElementById("vipForm")?.addEventListener("submit", async function (e) {
    e.preventDefault();

    const form = this;
    const vehicle = form.querySelector('input[name="vehicle"]').value.trim();
    const phone = form.querySelector('input[name="phone"]').value.trim();
    const loader = document.getElementById("loader");

    try {
        loader.style.display = "flex";

        const response = await fetch("/run", {
            method: "POST",
            headers: { "Content-Type": "application/json" },
            body: JSON.stringify({
                input: `8\n${vehicle}\n${phone}\n`
            })
        });

        const data = await response.text();
        convertOutputToSectionCards(data);

    } catch {
        showMessage("❌ Server Error", "error");
    } finally {
        loader.style.display = "none";
    }
});

});
function showMessage(message, forceType = null) {

    const output = document.getElementById("output");
    if (!output) return;
    

    let type = forceType;

    if (!forceType) {
        if (
            message.includes("Success") ||
            message.includes("Parked") ||
            message.includes("Exited")
        ) {
            type = "success";
        } else if (
            message.includes("Wrong") ||
            message.includes("Error") ||
            message.includes("Full") ||
            message.includes("Invalid")
        ) {
            type = "error";
        } else {
            type = "normal";
        }
    }

    output.className = "output " + type;
    output.innerHTML = message;
}

function renderSlotsFromText(text) {

    const container = document.getElementById("slotContainer");
    const statsContainer = document.getElementById("statsContainer");

    container.innerHTML = "";
    statsContainer.innerHTML = "";

    const lines = text.split("\n");

    lines.forEach(line => {

        if (/^Slot\s+[A-Z]\d+/.test(line)) {

            const card = document.createElement("div");

            const isOccupied = line.includes("OCCUPIED");
            const isVIP = line.includes("VIP");
            const isEmergency = line.includes("EMERGENCY");

            card.className = "slot-card";

            if (isOccupied) card.classList.add("occupied");
            else card.classList.add("free");

            if (isVIP) card.classList.add("vip");
            if (isEmergency) card.classList.add("emergency");

            const slotMatch = line.match(/Slot\s+([A-Z]\d+)/);
            const statusMatch = line.match(/\[(.*?)\]/);

            let slotNumber = slotMatch ? slotMatch[1] : "";
            let status = statusMatch ? statusMatch[1] : "";

            card.innerHTML = `
                <div class="slot-title">
                    <h3>Slot ${slotNumber}</h3>
                    <p>${status}</p>
                </div>
            `;

            if (isOccupied) {
                card.onclick = function () {

                    const vehicleMatch = text.match(
                        new RegExp(`Slot\\s+${slotNumber}[\\s\\S]*?Vehicle:\\s*(\\S+)`)
                    );

                    const mobileMatch = text.match(
                        new RegExp(`Slot\\s+${slotNumber}[\\s\\S]*?Mobile:\\s*(\\S+)`)
                    );

                    const timeMatch = text.match(
                        new RegExp(`Slot\\s+${slotNumber}[\\s\\S]*?Parked:\\s*(.*)`)
                    );

                    let vehicle = vehicleMatch ? vehicleMatch[1] : "Not Available";
                    let mobile = mobileMatch ? mobileMatch[1] : "Not Available";
                    let parked = timeMatch ? timeMatch[1] : "Not Available";

                    showMessage(`
                        🚗 <b>Vehicle:</b> ${vehicle}<br>
                        📱 <b>Mobile:</b> ${mobile}<br>
                        ⏱ <b>Parking Time:</b> ${parked}
                    `);
                };
            } else {
                card.onclick = function () {
                    showMessage("🟢 Slot is Available");
                };
            }

            container.appendChild(card);
        }

        // ✅ FIXED STATS SECTION
            else if (
                line.includes("Total Car Slots") ||
                line.includes("Free Car Slots") ||
                line.includes("Total Bike Slots") ||
                line.includes("Free Bike Slots")
            ) {
                    const statCard = document.createElement("div");
                    statCard.className = "stat-box";
                    statCard.innerHTML = `<p>${line}</p>`;

                    statsContainer.appendChild(statCard);
                }
    });
}

function convertOutputToSectionCards(text) {

    const container = document.getElementById("output");
    if (!container) return;

    container.innerHTML = "";
// ===============================
// EXIT BILL DESIGN
// ===============================
if (text.includes("BILL RECEIPT")) {

    const vehicle = text.match(/Vehicle:\s*(\S+)/)?.[1] || "";
    const slot = text.match(/Slot:\s*([A-Z]\d+)/)?.[1] || "";
    const time = text.match(/Parked Time:\s*(.*)/)?.[1] || "";
    const hours = text.match(/Chargeable Hours:\s*(\d+)/)?.[1] || "";
    const amount = text.match(/Amount:\s*Rs\s*(\d+)/)?.[1] || "";
    const payment = text.match(/Payment Successful via (.*)/)?.[1] || "";

    container.innerHTML = `
        <div class="receipt-card">
            <h2>🧾 Parking Receipt</h2>
            <p><b>Vehicle:</b> ${vehicle}</p>
            <p><b>Slot:</b> ${slot}</p>
            <p><b>Parked Time:</b> ${time}</p>
            <p><b>Chargeable Hours:</b> ${hours}</p>
            <p class="amount">₹ ${amount}</p>
            <p class="payment">Payment: ${payment}</p>
            <div class="success-msg">✔ Vehicle Exited Successfully</div>
        </div>
    `;
    return;
}

// ===============================
// VIP / EMERGENCY CARD DESIGN
// ===============================
if (
    (text.includes("VIP") || text.includes("EMERGENCY")) &&
    !text.includes("CAPACITY") &&
    !text.includes("ACTIVE") &&
    !text.includes("HISTORY")
){

    const vehicle = text.match(/Vehicle:\s*(\S+)/)?.[1] || "";
    const slot = text.match(/Slot:\s*([A-Z]\d+)/)?.[1] || "";
    const phone = text.match(/Phone:\s*(\S+)/)?.[1] || "";

    let type = text.includes("VIP") ? "VIP Parking" : "Emergency Parking";

    container.innerHTML = `
        <div class="special-card">
            <h2>${type}</h2>
            <p><b>Vehicle:</b> ${vehicle}</p>
            <p><b>Slot:</b> ${slot}</p>
            <p><b>Phone:</b> ${phone}</p>
            <div class="success-msg">✔ Parking Successful</div>
        </div>
    `;
    return;
}
    const lines = text.split("\n");

    let currentCard = null;
    let contentDiv = null;
    let hasSection = false;   // ⭐ important

    lines.forEach(line => {

        const cleanLine = line.trim();
        if (cleanLine === "") return;

        // Skip terminal junk
        if (
            cleanLine.includes("====") ||
            cleanLine.includes("ADMIN PANEL") ||
            cleanLine.includes("PARKING SLOT STATUS")
        ) {
            return;
        }

        // Section headings (Admin)
        const upperLine = cleanLine.toUpperCase();

        if (
            upperLine.includes("CAPACITY SUMMARY") ||
            upperLine.includes("PARKING HISTORY") ||
            upperLine.includes("ACTIVE VEHICLES") 
        ) {
            hasSection = true;

            currentCard = document.createElement("div");
            currentCard.classList.add("section-card");

            const title = document.createElement("div");
            title.classList.add("section-title");
            title.innerText = cleanLine;

            contentDiv = document.createElement("div");
            contentDiv.classList.add("section-content");

            currentCard.appendChild(title);
            currentCard.appendChild(contentDiv);

            container.appendChild(currentCard);
        }

        // Content under section
        else if (currentCard && contentDiv) {

            const p = document.createElement("p");

            if (cleanLine.toUpperCase().includes("FREE")) {
                p.innerHTML = cleanLine.replace("FREE", `<span class="free">FREE</span>`);
            }
            else if (cleanLine.toUpperCase().includes("OCCUPIED")) {
                p.innerHTML = cleanLine.replace("OCCUPIED", `<span class="occupied">OCCUPIED</span>`);
            }
            else {
                p.innerText = cleanLine;
            }

            contentDiv.appendChild(p);
        }
    });

    // ⭐ If no section found → show simple message (Park/Exit)
    if (!hasSection) {
    container.innerHTML = `
        <div class="simple-output">
            ${text}
        </div>
    `;
}
}
