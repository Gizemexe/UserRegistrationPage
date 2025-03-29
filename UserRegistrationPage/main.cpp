#include "icb_gui.h"
#include "ic_media.h"
#include "icbytes.h"

int MLE; // Çok satýrlý metin alaný
int NameInput, SurnameInput, PhoneInput, EmailInput, PasswordInput;
int RegisterButton, UploadPhotoButton, PhotoFrame;

struct VERI_TABANI {
    ICDEVICE index_dosya, veri_dosya;
    ICBYTES index, anahtar, bilgi;
    unsigned burayabak = 1;
};

static VERI_TABANI veritaban;

unsigned IndexAra(ICBYTES& index, unsigned long long* map) {
    for (unsigned i = 1; i <= index.Y(); i++) {
        if (index.O(1, i) == map[0] && index.O(2, i) == map[1]) return i;
    }
    return 0xffffffff;
}

void RegisterCustomer(void* p = nullptr)
{
    ICBYTES ad, soyad, telefon, email, sifre, bilgi, virgul, newline;
    virgul = ","; newline = "\n";

    GetText(NameInput, ad);
    GetText(SurnameInput, soyad);
    GetText(PhoneInput, telefon);
    GetText(EmailInput, email);
    GetText(PasswordInput, sifre);

    veritaban.anahtar = ad; veritaban.anahtar += soyad;

    bilgi = ad; bilgi += virgul;
    bilgi += soyad; bilgi += virgul;
    bilgi += telefon; bilgi += virgul;
    bilgi += email; bilgi += virgul;
    bilgi += sifre; bilgi += newline;

    veritaban.bilgi = bilgi;

    unsigned long long* map = KeyMapTR(veritaban.anahtar, 2);
    if (!map) {
        ICG_printf(MLE, "Anahtar üretilemedi!\n");
        return;
    }
    unsigned hangisi = IndexAra(veritaban.index, map);

    if (hangisi != 0xffffffff) {
        ICG_printf(MLE, "Bu müþteri zaten kayýtlý!\n");
        return;
    }

    long long sonindex = veritaban.index.Y();

    if (!(veritaban.index.Y() == 1 && veritaban.index.O(1, 1) == 0)) {
        sonindex++;
        ResizeMatrix(veritaban.index, 3, sonindex);
    }

    veritaban.index.O(1, sonindex) = map[0];
    veritaban.index.O(2, sonindex) = map[1];
    veritaban.index.O(3, sonindex) = GetFileLength(veritaban.veri_dosya);

    long long newadd = WriteICBYTES(veritaban.veri_dosya, veritaban.anahtar, veritaban.index.O(3, sonindex));
    WriteICBYTES(veritaban.veri_dosya, veritaban.bilgi, newadd);

    ICG_printf(MLE, "Müþteri kaydedildi:\n");
    Print(MLE, veritaban.bilgi);
}

void UploadPhoto() {}

void Baslama(VERI_TABANI& v) {
    CreateMatrix(v.index, 3, 1, ICB_ULONGLONG);
    v.index = 0;
    CreateFileDevice(v.index_dosya, "index.bin");
    CreateFileDevice(v.veri_dosya, "veri.bin");
    ReadICBYTES(v.index_dosya, v.index, 0);
}

void ExitFonksiyonu(void* t) {
    VERI_TABANI& veritaban = *((VERI_TABANI*)t);
    if (veritaban.index.O(1, 1) != 0)
        WriteICBYTES(veritaban.index_dosya, veritaban.index, 0);
    CloseDevice(veritaban.index_dosya);
    CloseDevice(veritaban.veri_dosya);
}

void ICGUI_Create() {
    ICG_MWSize(600, 500);
    ICG_MWTitle("Müþteri Kayýt Formu");
}

void ICGUI_main() {
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
    MLE = ICG_MLEditSunken(50, 290, 500, 150, "", SCROLLBAR_V);

    RegisterButton = ICG_Button(150, 220, 100, 30, "Kayýt Ekle", RegisterCustomer, nullptr);
    UploadPhotoButton = ICG_Button(400, 180, 100, 40, "Fotoðraf Yükle", UploadPhoto);

    PhotoFrame = ICG_FrameThick(380, 20, 150, 150);

    ICG_SetOnExit(ExitFonksiyonu, &veritaban);
    Baslama(veritaban);
}
