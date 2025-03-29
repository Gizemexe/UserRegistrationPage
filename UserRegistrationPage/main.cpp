#include "icb_gui.h"
#include "ic_media.h"
#include "icbytes.h"



int MLE; // Çok satýrlý metin alaný
int NameInput, SurnameInput, PhoneInput, EmailInput, PasswordInput;
int RegisterButton, UploadPhotoButton, PhotoFrame, DatabaseView; 
;

ICBYTES Kayitlar;


// Buton fonksiyonlarý
void RegisterCustomer()
{
    ICBYTES ad, soyad, telefon, email, password,  kayit, virgul, newline;
    virgul = ",";
    newline = "\n";

    GetText(NameInput, ad);
    GetText(SurnameInput, soyad);
    GetText(PhoneInput, telefon);
    GetText(EmailInput, email);
    GetText(PasswordInput, password);

    kayit = ad;
    kayit += virgul;
    kayit += soyad;
    kayit += virgul;
    kayit += telefon;
    kayit += virgul;
    kayit += email;
    kayit += virgul;
    kayit += password;
    kayit += newline;

    Kayitlar += kayit;
    SetText(DatabaseView, Kayitlar);

    Print(MLE, kayit);
}


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
    PasswordInput = ICG_SLPasswordB(150, 180, 200, 25);

    ICG_MLStatic(50, 270, 200, 20, "Kayýtlý Veriler:");
    DatabaseView = ICG_MLEditSunken(50, 290, 500, 150, "", SCROLLBAR_V);

    RegisterButton = ICG_Button(150, 220, 100, 30, "Kayýt Ekle", RegisterCustomer);
    UploadPhotoButton = ICG_Button(400, 180, 100, 40, "Fotoðraf Yükle", UploadPhoto);

    PhotoFrame = ICG_FrameThick(380, 20, 150, 150); // Fotoðraf önizleme alaný
}


