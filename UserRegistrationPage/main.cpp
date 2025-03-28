#include "icb_gui.h"

int MLE; // �ok sat�rl� metin alan�
int NameInput, SurnameInput, PhoneInput, EmailInput;
int RegisterButton, UploadPhotoButton, PhotoFrame;

// Buton fonksiyonlar�
void RegisterCustomer() {}
void UploadPhoto() {}


void ICGUI_Create()
{
    ICG_MWSize(600, 500); // Pencere boyutu
    ICG_MWTitle("M��teri Kay�t Formu"); // Ba�l�k
}

void ICGUI_main()
{
    ICGUI_Create();

    ICG_Static(50, 20, 80, 25, "Ad:");
    NameInput = ICG_SLEditSunken(150, 20, 200, 25, "");

    ICG_Static(50, 60, 80, 25, "Soyad:");
    SurnameInput = ICG_SLEditSunken(150, 60, 200, 25, "");

    ICG_Static(50, 100, 80, 25, "Telefon:");
    PhoneInput = ICG_SLEditSunken(150, 100, 200, 25, "");

    ICG_Static(50, 140, 80, 25, "E-posta:");
    EmailInput = ICG_SLEditSunken(150, 140, 200, 25, "");

    RegisterButton = ICG_Button(150, 180, 100, 30, "Kay�t Ekle", RegisterCustomer);
    UploadPhotoButton = ICG_Button(400, 180, 100, 30, "Foto�raf Y�kle", UploadPhoto);

    PhotoFrame = ICG_FramePanel(380, 20, 150, 150); // Foto�raf �nizleme alan�
}


