#include <iostream>
#include <vector>
#include <fstream>
#include <cmath>
#include <ctime>
#include <cstdlib>
#include <regex>
#include <algorithm>
#include <iomanip>

using namespace std;

// ---------------- VEHICLE ----------------
class Vehicle {
protected:
    string vehicleNumber;
    time_t entryTime;

public:
    Vehicle(string num, time_t entry = time(0)) {
        vehicleNumber = num;
        entryTime = entry;
    }

    virtual float calculateCharge(int hours) = 0;
    string getVehicleNumber() { return vehicleNumber; }
    time_t getEntryTime() { return entryTime; }
    virtual int getType() = 0;
    virtual ~Vehicle() {}
};

class Car : public Vehicle {
    bool isVIP;
    bool isEmergency;

public:
    Car(string num, bool vip=false, bool emergency=false, time_t entry = time(0))
        : Vehicle(num, entry)
    {
        isVIP = vip;
        isEmergency = emergency;
    }

    float calculateCharge(int hours) {

        if(isVIP)
            return hours * 100;     // VIP Price

        if(isEmergency)
            return hours * 80;      // Emergency Price

        return hours * 50;          // Normal Price
    }

    int getType() { return 1; }
};

class Bike : public Vehicle {
public:
    Bike(string num, time_t entry = time(0)) : Vehicle(num, entry) {}
    float calculateCharge(int hours) { return hours * 20; }
    int getType() { return 2; }
};

class ParkingSlot {
    string slotID;
    int type;       // 1 = Car, 2 = Bike
    bool occupied;
    bool vipSlot;
    bool emergencySlot;

public:
    ParkingSlot(string id, int t, bool vip=false, bool emergency=false) {
        slotID = id;
        type = t;
        occupied = false;
        vipSlot = vip;
        emergencySlot = emergency;
    }

    bool isOccupied() { return occupied; }
    int getType() { return type; }
    string getSlotID() { return slotID; }
    bool isVIP() { return vipSlot; }
    bool isEmergency() { return emergencySlot; }

    void occupy() { occupied = true; }
    void release() { occupied = false; }
};     

class Ticket {
private:
    string slotID;
    Vehicle* vehicle;
    string mobile;
    int otp;
    time_t entryTime;   // store entry time in ticket

public:

    Ticket(string slot, Vehicle* v, string mob, time_t entry, int existingOtp = -1) {
    slotID = slot;
    vehicle = v;
    mobile = mob;
    entryTime = entry;   // now entry is properly defined

    if (existingOtp == -1) {
        otp = 1000 + rand() % 9000;
        cout << "\nOTP for Exit: " << otp << endl;
    } else {
        otp = existingOtp;
    }
}

    string getMobile() { return mobile; }
    string getVehicleNumber() { return vehicle->getVehicleNumber(); }
    string getSlotID() { return slotID; }
    int getType() { return vehicle->getType(); }
    time_t getEntryTime() { return entryTime; }
    int getOTP() { return otp; }

    bool verifyOTP(int entered) {
        return entered == otp;
    }

    float closeTicket(int paymentChoice) {

        time_t exitTime = time(0);

        double seconds = difftime(exitTime, entryTime);
        double totalHours = seconds / 3600.0;

        int chargeableHours = ceil(totalHours);
        if (chargeableHours <= 0) chargeableHours = 1;

        float charge = vehicle->calculateCharge(chargeableHours);

        if(chargeableHours > 12) {
            charge += 100;
            cout << "Overstay Fine Applied: Rs 100\n";
        }

        string payment = (paymentChoice == 1) ? "UPI" :
                         (paymentChoice == 2) ? "Card" : "Cash";

        cout << "\nPayment Successful via " << payment << endl;

        int totalMinutes = seconds / 60;
        int hrs = totalMinutes / 60;
        int mins = totalMinutes % 60;

        cout << "\n------ BILL RECEIPT ------\n";
        cout << "Vehicle: " << vehicle->getVehicleNumber() << endl;
        cout << "Slot: " << slotID << endl;
        cout << "Parked Time: " << hrs << " hr " << mins << " min\n";
        cout << "Chargeable Hours: " << chargeableHours << endl;
        cout << "Amount: Rs " << charge << endl;
        cout << "--------------------------\n";

        return charge;
    }

    ~Ticket() { delete vehicle; }
};

// ---------------- SYSTEM ----------------
class ParkingSystem {

    vector<ParkingSlot> slots;
    vector<Ticket*> activeTickets;
    string adminPassword = "admin123";
    float totalRevenue = 0;

public:

    ParkingSystem() {

        srand(time(0));

        ifstream revFile("revenue.txt");
        if(revFile)
            revFile >> totalRevenue;
        revFile.close();

        // CAR SLOTS
        for(int i=1;i<=10;i++) {

            string slot = "A" + to_string(i);

            if(i==1)
                slots.push_back(ParkingSlot(slot,1,true,false));   // VIP
            else if(i==2)
                slots.push_back(ParkingSlot(slot,1,false,true));   // Emergency
            else
                slots.push_back(ParkingSlot(slot,1));
        }

        // BIKE SLOTS
        for(int i=1;i<=10;i++) {

            string slot = "B" + to_string(i);
            slots.push_back(ParkingSlot(slot,2));
        }

        loadFromFile();
    }

    void showRevenue() {

    cout << "Content-Type: application/json\n\n";
    cout << "{";
    cout << "\"totalRevenue\": " << totalRevenue;
    cout << "}";
    }

    void saveRevenue() {
        ofstream file("revenue.txt");
        file << totalRevenue;
        file.close();
    }

    void saveToFile() {

        ofstream file("data.txt");

        for (auto t : activeTickets) {
        file << t->getVehicleNumber() << " "
                << t->getSlotID() << " "
                << t->getType() << " "
                << t->getEntryTime() << " "
                << t->getOTP() << " "
                << t->getMobile() << "\n";
        }

        file.close();
    }

void loadFromFile() {

    ifstream file("data.txt");
    if (!file) return;

    string number, slotID, mobile;
    int type, otp;
    time_t entry;

    while (file >> number >> slotID >> type >> entry >> otp>> mobile) {
        Vehicle* v;
        bool vip = false;
        bool emergency = false;

        for (auto &slot : slots) {
            if (slot.getSlotID() == slotID) {
                if (slot.isVIP()) vip = true;
                if (slot.isEmergency()) emergency = true;
                slot.occupy();
            }
        }
        if(type == 1)
            v = new Car(number, false, false, entry);
        else
            v = new Bike(number, entry);
        activeTickets.push_back(new Ticket(slotID, v, mobile, entry, otp));
    }

    file.close();
}

    bool isValidIndianNumber(string number) {
        regex pattern("^[A-Z]{2}[0-9]{2}[A-Z]{2}[0-9]{4}$");
        return regex_match(number, pattern);
    }

    bool isValidMobile(string mobile) {
    regex pattern("^[6-9][0-9]{9}$");
    return regex_match(mobile, pattern);
}

    bool isDuplicate(string number) {
        for (auto t : activeTickets)
            if (t->getVehicleNumber() == number)
                return true;
        return false;
    }

    void parkVehicle(string number, int type, string mobile) {
      
        transform(number.begin(), number.end(), number.begin(), ::toupper);

        if(!isValidMobile(mobile)) {
            cout << "Invalid Indian Mobile Number!\n";
            return;
        }

        if (!isValidIndianNumber(number)) {
            cout << "Invalid Indian Vehicle Number Format!\n";
            return;
        }

        if (isDuplicate(number)) {
            cout << "Duplicate Vehicle Entry!\n";
            return;
        }

        for (auto &slot : slots) {

             if (!slot.isOccupied() &&
                 slot.getType() == type &&
                !slot.isVIP() &&
                !slot.isEmergency()) {

                    Vehicle* v = (type == 1) ?
                    (Vehicle*) new Car(number, false, false) :  // Normal Car
                    (Vehicle*) new Bike(number);

                slot.occupy();
                activeTickets.push_back(new Ticket(slot.getSlotID(), v, mobile, time(0)));
                saveToFile();

                cout << "Vehicle Parked at Slot "
                     << slot.getSlotID() << endl;

                return;
            }
        }
        cout << "Parking Full!\n";
    }

    void exitVehicle(string number, int enteredOTP, int paymentChoice) {

        transform(number.begin(), number.end(), number.begin(), ::toupper);

        for (int i = 0; i < activeTickets.size(); i++) {

            if (activeTickets[i]->getVehicleNumber() == number) {

                if (!activeTickets[i]->verifyOTP(enteredOTP)) {
                    cout << "Wrong OTP!\n";
                    return;
                }

                // ✅ STORE DATA BEFORE DELETE
                string vehicleNumber = activeTickets[i]->getVehicleNumber();
                string slotID = activeTickets[i]->getSlotID();
                time_t exitTime = time(0);

                float amount = activeTickets[i]->closeTicket(paymentChoice);
                totalRevenue += amount;
                saveRevenue();
                updateDailyRevenue(amount);

                // ✅ SAVE TO HISTORY FILE
                ofstream history("history.txt", ios::app);
                history << vehicleNumber << " "
                        << slotID << " "
                        << exitTime << "\n";
                history.close();

                // release slot
                for (auto &slot : slots)
                    if (slot.getSlotID() == slotID)
                        slot.release();

                // delete ticket safely
                delete activeTickets[i];
                activeTickets.erase(activeTickets.begin() + i);

                saveToFile();

                cout << "Vehicle Exited Successfully!\n";
                return;
            }
        }

        cout << "Vehicle Not Found!\n";
    }

    void adminForceExit(string number) {

    number.erase(remove(number.begin(), number.end(), ' '), number.end());
    number.erase(remove(number.begin(), number.end(), '-'), number.end());
    transform(number.begin(), number.end(), number.begin(), ::toupper);

    for (int i = 0; i < activeTickets.size(); i++) {

        if (activeTickets[i]->getVehicleNumber() == number) {

            cout << "\n⚠ ADMIN FORCE EXIT INITIATED ⚠\n";

            // Store details before deleting
            string vehicleNumber = activeTickets[i]->getVehicleNumber();
            string slotID = activeTickets[i]->getSlotID();
            time_t exitTime = time(0);

            // Close ticket with Cash by default (3)
            float amount = activeTickets[i]->closeTicket(3);
            totalRevenue += amount;
            saveRevenue();

            // Save history
            ofstream history("history.txt", ios::app);
            history << vehicleNumber << " "
                    << slotID << " "
                    << exitTime << "\n";
            history.close();

            // Release slot
            for (auto &slot : slots)
                if (slot.getSlotID() == slotID)
                    slot.release();

            delete activeTickets[i];
            activeTickets.erase(activeTickets.begin() + i);

            saveToFile();

            cout << "Vehicle Force Removed Successfully!\n";
            return;
        }
    }

    cout << "Vehicle Not Found!\n";
}

    void showHistory() {

    ifstream file("history.txt");

    if (!file) {
        cout << "No History Found\n";
        return;
    }

    string number;
    string slot;
    time_t exitTime;

    cout << "\n----- PARKING HISTORY -----\n";

    while (file >> number >> slot >> exitTime) {
        cout << "Vehicle: " << number
             << " | Slot: " << slot
             << " | Exit Time: "
             << ctime(&exitTime);
    }

    file.close();
}

  void adminLogin(string password) {

    if(password != adminPassword) {
        cout << "Wrong Password\n";
        return;
    }

    cout << "\n========= ADMIN PANEL =========\n";

    showSlots();
    showHistory();
    showDailyRevenue();

    cout << "\nActive Vehicles:\n";

    for(auto t : activeTickets) {
        cout << "Vehicle: "
             << t->getVehicleNumber()
             << " | Slot: "
             << t->getSlotID()
             << endl;
    }
             cout << "\nTotal Revenue: Rs " << totalRevenue << endl;
}

    void showSlots() {

        int freeCar = 0, freeBike = 0;

        cout << "=====================================\n";
        cout << "         PARKING SLOT STATUS         \n";
        cout << "=====================================\n\n";

        cout << "--------------- CAR SLOTS ---------------\n";

        for (auto &slot : slots)
        {
            if (slot.getType() == 1)
            {
                cout << "Slot " << slot.getSlotID();

                if (slot.isVIP())
                    cout << " (VIP)";
                else if (slot.isEmergency())
                    cout << " (EMERGENCY)";

                if (slot.isOccupied())
                {
                    cout << " : [ OCCUPIED ]";

                    for (auto t : activeTickets)
                    {
                        if (t->getSlotID() == slot.getSlotID())
                        {
                            time_t now = time(0);
                            double seconds = difftime(now, t->getEntryTime());
                            int mins = seconds / 60;
                            int secs = (int)seconds % 60;

                            cout << " | Vehicle: " << t->getVehicleNumber();
                            cout << " | Mobile: " << t->getMobile();
                            cout << " | Parked: " << mins << " mins " << secs << " secs";
                        }
                    }
                }
                else
                {
                    cout << " : [ FREE ]";
                    freeCar++;
                }

                cout << endl;
            }
        }

        cout << "\n--------------- BIKE SLOTS --------------\n";

        for (auto &slot : slots)
        {
            if (slot.getType() == 2)
            {
                cout << "Slot " << slot.getSlotID();

                if (slot.isOccupied())
                {
                    cout << " : [ OCCUPIED ]";

                    for (auto t : activeTickets)
                    {
                        if (t->getSlotID() == slot.getSlotID())
                        {
                            time_t now = time(0);
                            double seconds = difftime(now, t->getEntryTime());
                            int mins = seconds / 60;
                            int secs = (int)seconds % 60;

                            cout << " | Vehicle: " << t->getVehicleNumber();
                            cout << " | Mobile: " << t->getMobile();
                            cout << " | Parked: " << mins << " mins " << secs << " secs";
                        }
                    }
                } 
                else
                {
                    cout << " : [ FREE ]";
                    freeBike++;
                }

                cout << endl;
            }
        }

        cout << "\n=====================================\n";
        cout << "          CAPACITY SUMMARY           \n";
        cout << "=====================================\n";
        int totalCar = 0, totalBike = 0;

        for (auto &slot : slots) {
            if (slot.getType() == 1) totalCar++;
            if (slot.getType() == 2) totalBike++;
        }

        cout << "Total Car Parked          : " << totalCar << endl;
        cout << "Free Car Slots           : " << freeCar << endl;
        cout << "Total Bike Slots         : " << totalBike << endl;
        cout << "Free Bike Slots          : " << freeBike << endl;
        cout << "=====================================\n";
    }

    void searchVehicle(string number) {

    transform(number.begin(), number.end(), number.begin(), ::toupper);

    for (auto t : activeTickets)
        if (t->getVehicleNumber() == number) {
            cout << "Vehicle Found in Slot "
                 << t->getSlotID() << endl;
            return;
        }

    cout << "Vehicle Not Found!\n";
}   

void emergencyPark(string number, string mobile) {

    transform(number.begin(), number.end(), number.begin(), ::toupper);

    for(auto &slot : slots) {

        if (isDuplicate(number)) {
            cout << "Duplicate Vehicle Entry!\n";
            return;
        }
        if(!isValidMobile(mobile)) {
            cout << "Invalid Indian Mobile Number!\n";
            return;
        }

        if(slot.isEmergency() && !slot.isOccupied()) {

           Vehicle* v = new Car(number, false, true);
            slot.occupy();

            activeTickets.push_back(new Ticket(slot.getSlotID(), v, mobile, time(0)));

            saveToFile();

            cout << "Emergency Vehicle Parked at Emergency Slot "
                 << slot.getSlotID() << endl;

            return;
        }
    }

    cout << "Emergency Slot Occupied!\n";
}

void vipPark(string number, string mobile) {

    transform(number.begin(), number.end(), number.begin(), ::toupper);

    for(auto &slot : slots) {

        if(!isValidMobile(mobile)) {
            cout << "Invalid Indian Mobile Number!\n";
            return;
        }
        if (isDuplicate(number)) {
            cout << "Duplicate Vehicle Entry!\n";
            return;
        }

        if(slot.isVIP() && !slot.isOccupied()) {

            Vehicle* v = new Car(number, true, false); // VIP true
            slot.occupy();

            activeTickets.push_back(new Ticket(slot.getSlotID(), v, mobile, time(0)));

            saveToFile();

            cout << "VIP Vehicle Parked at VIP Slot "
                 << slot.getSlotID() << endl;

            return;
        }
    }

    cout << "VIP Slot Occupied!\n";
}

void showDashboardStats() {

    int totalVehicles = activeTickets.size();
    int freeSlots = 0;

    for (auto &slot : slots)
        if (!slot.isOccupied())
            freeSlots++;

    cout << "Total Vehicles: " << totalVehicles << endl;
    cout << "Available Slots: " << freeSlots << endl;
}

void updateDailyRevenue(float amount) {

    time_t now = time(0);
    tm *ltm = localtime(&now);

    char today[11];
    sprintf(today,"%04d-%02d-%02d",
            1900 + ltm->tm_year,
            1 + ltm->tm_mon,
            ltm->tm_mday);

    vector<pair<string,float>> records;

    ifstream file("daily_revenue.txt");

    string date;
    float revenue;
    bool found = false;

    while(file >> date >> revenue) {

        if(date == today) {
            revenue += amount;
            found = true;
        }

        records.push_back({date,revenue});
    }

    file.close();

    if(!found)
        records.push_back({today,amount});

    ofstream out("daily_revenue.txt");

    for(auto r : records)
        out << r.first << " " << r.second << endl;

    out.close();
}

void showDailyRevenue() {

    ifstream file("daily_revenue.txt");

    if(!file) {
        cout << "No Revenue Data\n";
        return;
    }

    string date;
    float amount;

    cout << "\n------ DAILY REVENUE HISTORY ------\n";

    while(file >> date >> amount) {
        cout << "Date: " << date
             << " | Revenue: Rs " << amount << endl;
    }

    file.close();
}

};

// ---------------- MAIN ----------------
int main() {

    // srand(time(0));
    ParkingSystem parking;

    int choice;
    cin >> choice;

    if(choice == 1) {
    string number, mobile;
    int type;

    cin >> number >> type >> mobile;

    parking.parkVehicle(number, type, mobile);
}
    else if(choice == 2) {
        string number; int otp; int payment;
        cin >> number >> otp >> payment;
        parking.exitVehicle(number, otp, payment);
    }
    else if(choice == 3) {
        parking.showSlots();
    }
    else if(choice == 4) {
        string number;
        cin >> number;
        parking.searchVehicle(number);
    }
    else if(choice == 5) {
        string pass;
        cin >> pass;
        parking.adminLogin(pass);
    }
  
    else if(choice == 6) {
    string number, mobile;
    cin >> number >> mobile;
    parking.emergencyPark(number, mobile);
    }
  
    else if(choice == 7) {

    string pass;
    cin >> pass;

    if(pass != "admin123") {
        cout << "Wrong Password\n";
        return 0;
    }

    string number;
    cin >> number;

    parking.adminForceExit(number);
    }

    else if(choice == 8) {
        string number, mobile;
        cin >> number >> mobile;
        parking.vipPark(number, mobile);
    }
   else if(choice == 9) {
        parking.showDashboardStats();
    }
    else if(choice == 10) {
        parking.showRevenue();
    }
    else if(choice == 11) {
        parking.showHistory();
    }   

    else {
        cout << "Invalid Choice\n";
    }

    return 0;
}