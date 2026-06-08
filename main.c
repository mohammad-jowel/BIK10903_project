#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_DOCTORS 20
#define MAX_APPOINTMENTS 100

typedef struct 
{
    int doctorId;
    char doctorName[50];
    char specialization[50];
} Doctor;

typedef struct 
{
    int appointmentId; 
    char patientName[50];
    char date[15];
    int doctorId;
    char reason[100];
} Appointment;


Doctor doctors[MAX_DOCTORS];
Appointment appointments[MAX_APPOINTMENTS];
int doctorCount = 0;
int appointmentCount = 0;

// Function prototypes
void loadDoctors();
void loadAppointments();
int  searchDoctor(int id);
int  findAppointment(int id);
void listDoctors();
void listAppointments();
int  comparePatientNames(const void *a, const void *b);
void insertAppointment();
void updateAppointment();
void deleteAppointment();
void saveAppointments();
int  isNotEmpty(const char *str);
void getTodayDate(char *buffer);


// Reads doctor records from doctors.csv into the doctors array, skipping the header.
void loadDoctors() 
{
    FILE *fp = fopen("doctors.csv", "r");
    if (!fp) {
        printf("Error opening doctors.csv\n");
        return;
    }
    char line[200];
    fgets(line, sizeof(line), fp); // skip header
    while (doctorCount < MAX_DOCTORS &&
           fscanf(fp, "%d,%49[^,],%49[^\n]\n",
                  &doctors[doctorCount].doctorId,
                  doctors[doctorCount].doctorName,
                  doctors[doctorCount].specialization) == 3) {
        doctorCount++;
    }
    fclose(fp);
}


// Reads appointment records from appointments.csv into the appointments array, skipping the header.
void loadAppointments() 
{
    FILE *fp = fopen("appointments.csv", "r");
    if (!fp) { 
        printf("Error opening appointments.csv\n"); 
        return; 
    }
    char line[300];
    fgets(line, sizeof(line), fp); // skip header
    while (appointmentCount < MAX_APPOINTMENTS &&
           fscanf(fp, "%d,%49[^,],%14[^,],%d,%99[^\n]\n",
                  &appointments[appointmentCount].appointmentId,
                  appointments[appointmentCount].patientName,
                  appointments[appointmentCount].date,
                  &appointments[appointmentCount].doctorId,
                  appointments[appointmentCount].reason) == 5) {
        appointmentCount++;
    }
    fclose(fp);
}


// Linearly searches doctors array by doctorId; returns index or -1 if not found.
int searchDoctor(int id) 
{
    for (int i = 0; i < doctorCount; i++) {
        if (doctors[i].doctorId == id) {
            return i;
        }
    }
    return -1;
}


// Binary searches appointments array by appointmentId; returns index or -1 if not found.
int findAppointment(int id)
{
    int low = 0, high = appointmentCount - 1;
    while (low <= high)
    {
        int mid = (low + high) / 2;
        if (appointments[mid].appointmentId == id)
            return mid;
        else if (appointments[mid].appointmentId < id)
            low = mid + 1;
        else
            high = mid - 1;
    }
    return -1;
}


// Returns 1 if string has non-whitespace content, 0 if blank.
int isNotEmpty(const char *str)
{
    while (*str) 
    {
        if (*str != ' ' && *str != '\t' && *str != '\n')
        {
            return 1;
        }
        str++;
    }
    return 0;
}


// qsort comparator: compares two Appointments by patientName (case-insensitive).
int comparePatientNames(const void *a, const void *b)
{
    const Appointment *ap = (const Appointment *)a;
    const Appointment *bp = (const Appointment *)b;
    return strcasecmp(ap->patientName, bp->patientName);
}


// Prints all doctors (ID, name, specialization) to the console.
void listDoctors() 
{
    printf("\n--- Doctor List ---\n");
    for (int i = 0; i < doctorCount; i++) {
        printf("ID: %d | Name: %s | Specialization: %s\n",
               doctors[i].doctorId,
               doctors[i].doctorName,
               doctors[i].specialization);
    }
}


// Sorts a copy of appointments alphabetically by patient name and displays it; original stays sorted by ID.
void listAppointments() 
{
    Appointment temp[MAX_APPOINTMENTS];
    memcpy(temp, appointments, appointmentCount * sizeof(Appointment));
    qsort(temp, appointmentCount, sizeof(Appointment), comparePatientNames);

    printf("\n--- Appointment List (Sorted by Patient Name) ---\n");
    for (int i = 0; i < appointmentCount; i++) {
        int dIndex = searchDoctor(temp[i].doctorId);
        printf("ID: %d | Patient: %s | Date: %s | Doctor: %s | Reason: %s\n",
               temp[i].appointmentId,
               temp[i].patientName,
               temp[i].date,
               (dIndex != -1 ? doctors[dIndex].doctorName : "Unknown"),
               temp[i].reason);
    }
}


// Fills buffer with today's date in YYYY-MM-DD format (buffer must be >= 11 bytes).
void getTodayDate(char *buffer)
{
    time_t t = time(NULL);
    struct tm *tm_info = localtime(&t);
    strftime(buffer, 11, "%Y-%m-%d", tm_info);
}


// Prompts for new appointment details, validates all input, and appends the record to the array.
void insertAppointment() 
{
    if (appointmentCount >= MAX_APPOINTMENTS) 
    {
        printf("Appointment list full!\n");
        return;
    }

    Appointment a;
    a.appointmentId = appointmentCount + 1;

    // Validate patient name
    printf("Enter Patient Name: ");
    scanf(" %[^\n]", a.patientName);
    if (!isNotEmpty(a.patientName)) {
        printf("Error: Patient name cannot be empty.\n");
        return;
    }

    // Auto-fill today's date
    getTodayDate(a.date);
    printf("Date auto-set to today: %s\n", a.date);

    // Validate doctor ID
    printf("Enter Doctor ID: ");
    if (scanf("%d", &a.doctorId) != 1) {
        printf("Error: Doctor ID must be a number.\n");
        while (getchar() != '\n'); // flush bad input
        return;
    }
    if (searchDoctor(a.doctorId) == -1) {
        printf("Error: Doctor ID %d not found.\n", a.doctorId);
        return;
    }

    // Validate reason
    printf("Enter Reason: ");
    scanf(" %[^\n]", a.reason);
    if (!isNotEmpty(a.reason)) {
        printf("Error: Reason cannot be empty.\n");
        return;
    }

    appointments[appointmentCount++] = a;
    printf("Appointment added successfully with ID %d.\n", a.appointmentId);
}


// Finds an appointment by ID and updates its name, doctor, and reason after validation.
void updateAppointment() 
{
    int id;
    printf("Enter Appointment ID to update: "); 
    scanf("%d", &id);

    int index = findAppointment(id);
    if (index == -1) { 
        printf("Error: Appointment ID %d not found.\n", id);
        return; 
    }

    // New patient name
    char newName[50];
    printf("Enter new Patient Name: ");
    scanf(" %[^\n]", newName);
    if (!isNotEmpty(newName)) {
        printf("Error: Patient name cannot be empty.\n");
        return;
    }

    // New doctor ID with existence check
    int newDoctorId;
    printf("Enter new Doctor ID: ");
    if (scanf("%d", &newDoctorId) != 1) {
        printf("Error: Doctor ID must be a number.\n");
        while (getchar() != '\n');
        return;
    }
    if (searchDoctor(newDoctorId) == -1) {
        printf("Error: Doctor ID %d not found.\n", newDoctorId);
        return;
    }

    // New reason
    char newReason[100];
    printf("Enter new Reason: ");
    scanf(" %[^\n]", newReason);
    if (!isNotEmpty(newReason)) {
        printf("Error: Reason cannot be empty.\n");
        return;
    }

    // All inputs valid — apply changes (date preserved from creation)
    strcpy(appointments[index].patientName, newName);
    appointments[index].doctorId = newDoctorId;
    strcpy(appointments[index].reason, newReason);

    printf("Appointment updated successfully.\n");
}


// Removes an appointment by ID, shifting subsequent records left.
void deleteAppointment() 
{
    int id;
    printf("Enter Appointment ID to delete: "); 
    scanf("%d", &id);

    int index = findAppointment(id);
    if (index == -1) { 
        printf("Error: Appointment ID %d not found.\n", id);
        return; 
    }

    for (int i = index; i < appointmentCount - 1; i++) {
        appointments[i] = appointments[i + 1];
    }
    appointmentCount--;
    printf("Appointment deleted successfully.\n");
}


// Overwrites appointments.csv with all current records, including the header row.
void saveAppointments() 
{
    FILE *fp = fopen("appointments.csv", "w");
    if (!fp) {
        printf("Error saving appointments.csv\n");
        return;
    }

    fprintf(fp, "AppointmentID,PatientName,Date,DoctorID,Reason\n");
    for (int i = 0; i < appointmentCount; i++) 
    {
        fprintf(fp, "%d,%s,%s,%d,%s\n",
                appointments[i].appointmentId,
                appointments[i].patientName,
                appointments[i].date,
                appointments[i].doctorId,
                appointments[i].reason);
    }
    fclose(fp);
    printf("Appointments saved successfully.\n");
}


// Loads CSV data then loops the menu until the user exits.
int main() 
{
    loadDoctors();
    loadAppointments();

    int choice;
    while (1)
    {
        printf("\n--- System for Pusat Kesihatan Universiti UTHM ---\n");
        printf("1. List Appointments\n");
        printf("2. Insert Appointment\n");
        printf("3. Update Appointment\n");
        printf("4. Delete Appointment\n");
        printf("5. List Doctors\n");
        printf("6. Exit\n");
        printf("Enter choice: ");
        scanf("%d", &choice);

        switch (choice)
        {
            case 1: 
                listAppointments();                          
                break;
            case 2: 
                insertAppointment();
                saveAppointments();
                break;
            case 3: 
                updateAppointment();
                saveAppointments();
                break;
            case 4: 
                deleteAppointment();
                saveAppointments();
                break;
            case 5: 
                listDoctors();                               
                break;
            case 6:
                saveAppointments();
                printf("Exiting...\n");                      
                return 0;
            default: 
                printf("Invalid choice. Please enter 1-6.\n");
        }
    }

    return 0;
}