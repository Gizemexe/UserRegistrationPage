#include "icb_gui.h"

int MLE; // Çok satýrlý metin alaný
int NameInput, SurnameInput, PhoneInput, EmailInput;
int RegisterButton, UploadPhotoButton, PhotoFrame;

// Buton fonksiyonlarý
void RegisterCustomer() {}
void UploadPhoto() {}


void ICGUI_Create()
{
    ICG_MWSize(600, 500); // Pencere boyutu
    ICG_MWTitle("Müþteri Kayýt Formu"); // Baþlýk
}

void ICGUI_main()
{
    ICGUI_Create();

    ICG_MLStatic(50, 20, 80, 25, "Ad:");
    NameInput = ICG_SLEditBorder(150, 20, 200, 25, "");

    ICG_MLStatic(50, 60, 80, 25, "Soyad:");
    SurnameInput = ICG_SLEditBorder(150, 60, 200, 25, "");

    ICG_MLStatic(50, 100, 80, 25, "Telefon:");
    PhoneInput = ICG_SLEditBorder(150, 100, 200, 25, "");

    ICG_MLStatic(50, 140, 80, 25, "E-posta:");
    EmailInput = ICG_SLEditBorder(150, 140, 200, 25, "");

    ICG_MLStatic(50, 180, 80, 25, "Þifre:");
    EmailInput = ICG_SLPasswordB(150, 180, 200, 25);

    RegisterButton = ICG_Button(150, 220, 100, 30, "Kayýt Ekle", RegisterCustomer);
    UploadPhotoButton = ICG_Button(400, 180, 100, 40, "Fotoðraf Yükle", UploadPhoto);

    PhotoFrame = ICG_FrameThick(380, 20, 150, 150); // Fotoðraf önizleme alaný
}


