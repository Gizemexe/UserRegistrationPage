#include "icb_gui.h"
#include "ic_media.h"
#include "icbytes.h"
#include <stdio.h>
#include <iostream> 

int MLE; // Çok satýrlý metin alaný
int NameInput, SurnameInput, PhoneInput, EmailInput, PasswordInput;
int RegisterButton, UploadPhotoButton, PhotoFrame;
int GenderInput, HealthProblemInput;

ICBYTES dosyaYolu, foto, kucultulmus, sonFoto;
int FRM;

ICBYTES arkaplan;
int ArkaPlanFrame;

ICBYTES PhotoPreview; 

struct VERI_TABANI {
    ICDEVICE index_dosya, veri_dosya;
    ICBYTES index, anahtar, bilgi;
    unsigned burayabak = 1;
};

void ICGUI_Create() {
    ICG_MWSize(900, 600);  // Pencereyi daha geniþ yapýyoruz
    ICG_MWTitle("Müþteri Kayýt Formu");
}

static VERI_TABANI veritaban;

unsigned IndexAra(ICBYTES& index, unsigned long long* map) {
    for (unsigned i = 1; i <= index.Y(); i++) {
        if (index.O(1, i) == map[0] && index.O(2, i) == map[1]) return i;
    }
    return 0xffffffff;
}

// Küçültme fonksiyonu
void ResmiKucult(ICBYTES& giris, ICBYTES& cikis, double oran) {
    int genislik = giris.X();
    int yukseklik = giris.Y();
    int kanal = giris.Z();

    int yeniGenislik = int(genislik * oran);
    int yeniYukseklik = int(yukseklik * oran);

    if (yeniGenislik < 1) yeniGenislik = 1;
    if (yeniYukseklik < 1) yeniYukseklik = 1;

    CreateMatrix(cikis, yeniGenislik, yeniYukseklik, kanal, ICB_UCHAR);

    for (int y = 1; y <= yeniYukseklik; y++) {
        for (int x = 1; x <= yeniGenislik; x++) {
            int eskiX = int((x - 1) / oran) + 1;
            int eskiY = int((y - 1) / oran) + 1;

            if (eskiX > genislik) eskiX = genislik;
            if (eskiY > yukseklik) eskiY = yukseklik;

            for (int z = 1; z <= kanal; z++) {
                cikis.B(x, y, z) = giris.B(eskiX, eskiY, z);
            }
        }
    }
}

// Orta bölgeyi alma fonksiyonu
void OrtaBolgeyiBul(ICBYTES& giris, ICBYTES& cikis, int bolge_genisligi, int bolge_yuksekligi) {
    int xBoy = giris.X();
    int yBoy = giris.Y();
    int zBoy = giris.Z();

    int baslangicX = (xBoy - bolge_genisligi) / 2 + 1;
    int baslangicY = (yBoy - bolge_yuksekligi) / 2 + 1;

    CreateMatrix(cikis, bolge_genisligi, bolge_yuksekligi, zBoy, ICB_UCHAR);

    for (int y = 1; y <= bolge_yuksekligi; y++) {
        for (int x = 1; x <= bolge_genisligi; x++) {
            for (int z = 1; z <= zBoy; z++) {
                cikis.B(x, y, z) = giris.B(baslangicX + x - 1, baslangicY + y - 1, z);
            }
        }
    }
}

void UploadPhoto() {
    char* yol = OpenFileMenu(dosyaYolu, "JPG\0*.JPG\0JPEG\0*.JPEG");
    if (yol) {
        ReadImage(yol, foto);

        // Küçültme oranýný hesapla
        double oranX = 150.0 / foto.X();
        double oranY = 150.0 / foto.Y();
        double oran = (oranX < oranY) ? oranX : oranY;

        // Küçült ve kýrp
        ResmiKucult(foto, kucultulmus, oran);
        OrtaBolgeyiBul(kucultulmus, sonFoto, 150, 150);

        // Göster
        DisplayImage(PhotoFrame, sonFoto);
        ICG_printf(MLE, "Fotoðraf yüklendi: %d x %d x %d\n", sonFoto.X(), sonFoto.Y(), sonFoto.Z());
    }
}

void RegisterCustomer(void* p = nullptr)
{
    ICBYTES ad, soyad, telefon, email, sifre, bilgi, virgul, newline, gender, healthProblem, healthDesc;
    virgul = ","; newline = "\n";

    ad = ""; soyad = ""; telefon = ""; email = ""; sifre = "";
    gender = ""; healthProblem = ""; healthDesc = "";
    veritaban.anahtar = ""; veritaban.bilgi = "";

    // Kullanýcýdan veri al
    GetText(NameInput, ad);
    GetText(SurnameInput, soyad);
    GetText(PhoneInput, telefon);
    GetText(EmailInput, email);
    GetText(PasswordInput, sifre);

    // Gender (Cinsiyet) seçeneði - Seçilen öðeyi al
    ICG_GetListItem(GenderInput, 0, gender);  // Gender seçilen öðe (0 indeksini kullandýk)

    // Health problem (Saðlýk problemi) seçeneði - Seçilen öðeyi al
    ICG_GetListItem(HealthProblemInput, 0, healthProblem);  // HealthProblem seçilen öðe (0 indeksini kullandýk)

    // Health Description (Saðlýk Açýklamasý)
    GetText(MLE, healthDesc);

    // Fotoðraf kontrolü
    if (sonFoto.X() == 0 || sonFoto.Y() == 0 || sonFoto.Z() == 0) {
        ICG_printf(MLE, "Lütfen bir fotoðraf yükleyin!\n");
        return;
    }

    veritaban.anahtar = ad; veritaban.anahtar += soyad;

    bilgi = ad; bilgi += virgul;
    bilgi += soyad; bilgi += virgul;
    bilgi += telefon; bilgi += virgul;
    bilgi += email; bilgi += virgul;
    bilgi += sifre; bilgi += virgul;
    bilgi += gender; bilgi += virgul;
    bilgi += healthProblem; bilgi += virgul;
    bilgi += healthDesc; bilgi += newline;

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
    if (veritaban.index.O(1, sonindex) != 0 || veritaban.index.Y() == 1) {
        sonindex++;
        ResizeMatrix(veritaban.index, 3, sonindex);
    }

    veritaban.index.O(1, sonindex) = map[0];
    veritaban.index.O(2, sonindex) = map[1];
    veritaban.index.O(3, sonindex) = GetFileLength(veritaban.veri_dosya);

    long long newadd = WriteICBYTES(veritaban.veri_dosya, veritaban.anahtar, veritaban.index.O(3, sonindex));
    newadd = WriteICBYTES(veritaban.veri_dosya, veritaban.bilgi, newadd);
    WriteICBYTES(veritaban.veri_dosya, sonFoto, newadd); // Fotoðraf kaydý

    ICG_printf(MLE, "Müþteri kaydedildi:\n");
    Print(MLE, veritaban.bilgi);

    // Fotoðrafý temizle (bir sonraki kayýtta zorunlu olsun)
    sonFoto = "";
    //DisplayImage(PhotoFrame, sonFoto); // alaný temizle
}

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

void DrawLine(ICBYTES& canvas, int x1, int y1, int x2, int y2, int color) {
    int dx = abs(x2 - x1);
    int dy = abs(y2 - y1);
    int sx = (x1 < x2) ? 1 : -1;
    int sy = (y1 < y2) ? 1 : -1;
    int err = dx - dy;

    float length = sqrt(dx * dx + dy * dy);
    float normX = (length == 0) ? 0 : -dy / length;
    float normY = (length == 0) ? 0 : dx / length;


    while (true) {
        // **Ýlk önce sadece ana çizgiyi çizer**
        Line(canvas, x1, y1, x2, y2, color);

        if (x1 == x2 && y1 == y2) break;

        int e2 = 2 * err;
        if (e2 > -dy) {
            err -= dy;
            x1 += sx;
        }
        if (e2 < dx) {
            err += dx;
            y1 += sy;
        }
    }
}

void DrawFilledCircle(ICBYTES& canvas, int cx, int cy, int radius, int color) {
    for (int y = -radius; y <= radius; y++) {
        int dx = (int)sqrt(radius * radius - y * y); // Dairenin yatay mesafesini hesapla
        FillRect(canvas, cx - dx, cy + y, 2 * dx, 1, color);  // Yatay çizgi çizer, daireyi doldurur
    }
}


void ICGUI_main() {
    ICGUI_Create();

    // Arkaplan 
    ReadImage("gym2.bmp", arkaplan);
    ArkaPlanFrame = ICG_FrameThick(0, 0, 900, 600);  
    
    ICG_StaticPanel(580, 50, 270, 40, "Take Steps For a Fit and Healthy Life!");

    // Form elemanlarý düzenlenmiþ þekilde
    ICG_StaticPanel(180, 10, 150, 30, "Registration Form");

    // Name, Surname, Phone, Email, Password
    ICG_StaticPanel(50, 50, 80, 30, "Name:");
    NameInput = ICG_SLEditThick(150, 50, 200, 30, "");

    ICG_StaticPanel(50, 90, 80, 30, "Surname:");
    SurnameInput = ICG_SLEditThick(150, 90, 200, 30, "");

    ICG_StaticPanel(50, 130, 80, 30, "Phone:");
    PhoneInput = ICG_SLEditThick(150, 130, 200, 30, "");

    ICG_StaticPanel(50, 170, 80, 30, "E-mail:");
    EmailInput = ICG_SLEditThick(150, 170, 200, 30, "");

    ICG_StaticPanel(50, 210, 80, 30, "Password:");
    PasswordInput = ICG_SLPasswordSunken(150, 210, 200, 30);

    // Gender
    ICG_StaticPanel(50, 250, 80, 30, "Gender:");
    GenderInput = ICG_ListBox(150, 255, 150, 30, nullptr);
    ICG_AddToList(GenderInput, "please select");
    ICG_AddToList(GenderInput, "Female");
    ICG_AddToList(GenderInput, "Male");
    ICG_AddToList(GenderInput, "Prefer not to say");

    // Health Problems
    ICG_StaticPanel(50, 290, 220, 30, "Do you have health problems?");
    HealthProblemInput = ICG_ListBox(280, 295, 110, 30, nullptr);
    ICG_AddToList(HealthProblemInput, "please select");
    ICG_AddToList(HealthProblemInput, "Yes");
    ICG_AddToList(HealthProblemInput, "No");

    // Health Description
    ICG_StaticPanel(50, 335, 250, 30, "Health Problem Description (if any):");
    MLE = ICG_MLEditSunken(50, 360, 500, 150, "", SCROLLBAR_V);

    // Register Button
    RegisterButton = ICG_Button(150, 515, 100, 30, "Register", RegisterCustomer, nullptr);
    UploadPhotoButton = ICG_Button(425, 225, 100, 40, "Upload Photo", UploadPhoto);

    // PhotoFrame
   
    CreateImage(PhotoPreview, 150, 150, 3, ICB_UCHAR);

    // Profil çerçevesi içine beyaz alan
    FillRect(PhotoPreview, 0, 0, 150, 150, 0xffffff);  // Beyaz renk ile çerçevenin içi doldurulur

    // Profil ikonu için daire çizimi
    int centerX = 75;  // Ortada X koordinatý
    int centerY = 75;  // Ortada Y koordinatý
    int radius = 35;   // Yarýçap
    int borderColor = 0x000000; // Siyah 

    // Profil icon
    DrawFilledCircle(PhotoPreview, centerX, centerY, 35, 0x000000);  // Daireyi çiziyoruz

    // Siyah dikdörtgen
    int rectColor = 0x000000; // Siyah 
    FillRect(PhotoPreview, centerX - 30, centerY + radius -5, 60, 70, rectColor);  // Dikdörtgen

    // Fotoðraf çerçevesi
    PhotoFrame = ICG_FrameThick(390, 50, 150, 150);

    // Fotoðrafý çerçeveye yerleþtirme
    DisplayImage(PhotoFrame, PhotoPreview);

    DisplayImage(ArkaPlanFrame, arkaplan);
    ICG_SetOnExit(ExitFonksiyonu, &veritaban);
    Baslama(veritaban);
}
