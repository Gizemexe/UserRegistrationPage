#include "icb_gui.h"
#include "ic_media.h"
#include "icbytes.h"
#include <stdio.h>
#include <iostream> 

int MLE, SLE; // Çok satırlı metin alanı
int NameInput, SurnameInput, PhoneInput, EmailInput, PasswordInput, ProblemInput;
int RegisterButton, UploadPhotoButton, PhotoFrame;
int GenderInput, HealthProblemInput;

ICBYTES dosyaYolu, foto, kucultulmus, sonFoto;
int FRM;

ICBYTES arkaplan;
int ArkaPlanFrame;

ICBYTES PhotoPreview;  // Renk önizleme alanı için bir matris
unsigned currentRecord = 1;

struct VERI_TABANI {
    ICDEVICE index_dosya, veri_dosya;
    ICBYTES index, anahtar, bilgi;
    unsigned burayabak = 1;
};

void ICGUI_Create() {
    ICG_MWSize(900, 600);  // Pencereyi daha geniş yapıyoruz
    ICG_MWTitle("Müşteri Kayıt Formu");
}

static VERI_TABANI veritaban;

unsigned IndexAra(ICBYTES& index, unsigned long long* map) {
    for (unsigned i = 1; i <= index.Y(); i++) {
        if (index.O(1, i) == map[0] && index.O(2, i) == map[1]) return i;
    }
    return 0xffffffff;
}
void NormalizeKey(ICBYTES& input, ICBYTES& output) {
    CreateMatrix(output, 1, 0, 1, ICB_CHAR);
    for (int i = 1; i <= input.Y(); i++) {
        char c = input.C(1, i);
        // Büyük harf küçük harfe çevir
        if (c >= 'A' && c <= 'Z') c += 32;

        // Eğer boşluksa atla
        if (c != ' ') output += c;
    }
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
    char* yol = OpenFileMenu(dosyaYolu, "JPG\0*.JPG\0JPEG\0*.JPEG\0");
    if (yol) {
        ReadImage(yol, foto);

        // Küçültme oranını hesapla
        double oranX = 150.0 / foto.X();
        double oranY = 150.0 / foto.Y();
        double oran = (oranX < oranY) ? oranX : oranY;

        // Küçült ve kırp
        ResmiKucult(foto, kucultulmus, oran);
        OrtaBolgeyiBul(kucultulmus, sonFoto, 150, 150);

        // Göster
        DisplayImage(PhotoFrame, sonFoto);
        ICG_printf(MLE, "Photograph is uploaded: %d x %d x %d\n", sonFoto.X(), sonFoto.Y(), sonFoto.Z());
    }
}

void RegisterCustomer(void* p = nullptr) {
    VERI_TABANI& vt = veritaban;
    ICBYTES name, surname, phone, email, password, Gender, Health, descp;
    ICBYTES temizAnahtar;

    GetText(NameInput, name);
    GetText(SurnameInput, surname);
    GetText(PhoneInput, phone);
    GetText(EmailInput, email);
    GetText(PasswordInput, password);
    GetText(GenderInput, Gender);
    GetText(HealthProblemInput, Health);
    GetText(ProblemInput, descp);


    if (sonFoto.X() == 0 || sonFoto.Y() == 0 || sonFoto.Z() == 0) {
        ICG_printf(MLE, "Please upload a photograph!\n");
        return;
    }

    NormalizeKey(name, temizAnahtar);
    unsigned long long* map = KeyMapTR(temizAnahtar, 2);

    long long sonindex = vt.index.Y();
    if (vt.index.O(1, sonindex) != 0) {
        sonindex++;
        ResizeMatrix(vt.index, 3, sonindex);
    }
    else {
        unsigned hangisi = IndexAra(vt.index, map);
        if (hangisi != 0xffffffff) {
            ICG_printf(MLE, "You are already registered!\n");
            return;
        }
    }

    vt.index.O(1, sonindex) = map[0];
    vt.index.O(2, sonindex) = map[1];
    vt.index.O(3, sonindex) = GetFileLength(vt.veri_dosya);

    long long addr = vt.index.O(3, sonindex);
    addr = WriteICBYTES(vt.veri_dosya, name, addr);
    addr = WriteICBYTES(vt.veri_dosya, surname, addr);
    addr = WriteICBYTES(vt.veri_dosya, phone, addr);
    addr = WriteICBYTES(vt.veri_dosya, email, addr);
    addr = WriteICBYTES(vt.veri_dosya, password, addr);
    addr = WriteICBYTES(vt.veri_dosya, Gender, addr);
    addr = WriteICBYTES(vt.veri_dosya, Health, addr);
    addr = WriteICBYTES(vt.veri_dosya, descp, addr);
    WriteICBYTES(vt.veri_dosya, sonFoto, addr);

    WriteICBYTES(vt.index_dosya, vt.index, 0);
    ICG_printf(MLE, "Kayıt başarılı!\n");
}

void DisplayRecord(void* p) {
    VERI_TABANI& vt = (*(VERI_TABANI*)p);
    if (currentRecord < 1 || currentRecord > vt.index.Y()) {
        ICG_printf(MLE, "Invalid record.\n");
        return;
    }

    ICBYTES name, surname, phone, email, password, gender, health,descp, foto;
    long long addr = vt.index.O(3, currentRecord);

    addr = ReadICBYTES(vt.veri_dosya, name, addr);
    addr = ReadICBYTES(vt.veri_dosya, surname, addr);
    addr = ReadICBYTES(vt.veri_dosya, phone, addr);
    addr = ReadICBYTES(vt.veri_dosya, email, addr);
    addr = ReadICBYTES(vt.veri_dosya, password, addr);
    addr = ReadICBYTES(vt.veri_dosya, gender, addr);
    addr = ReadICBYTES(vt.veri_dosya, health, addr);
    addr = ReadICBYTES(vt.veri_dosya, descp, addr);
    ReadICBYTES(vt.veri_dosya, foto, addr);

    ICG_SetWindowText(NameInput, &name.C(1));
    ICG_SetWindowText(SurnameInput, &surname.C(1));
    ICG_SetWindowText(PhoneInput, &phone.C(1));
    ICG_SetWindowText(EmailInput, &email.C(1));
    ICG_SetWindowText(PasswordInput, &password.C(1));
    ICG_SetWindowText(GenderInput, &gender.C(1));
    ICG_SetWindowText(HealthProblemInput, &health.C(1));
    ICG_SetWindowText(ProblemInput, &descp.C(1));
    DisplayImage(PhotoFrame, foto);
    sonFoto = foto;
}


bool EsitMi(ICBYTES& a, ICBYTES& b) {
    if (a.Y() != b.Y()) return false;  // Uzunluklar farklıysa eşit değil
    for (int i = 1; i <= a.Y(); i++) {
        if (a.C(1, i) != b.C(1, i)) return false;
    }
    return true;
}


void Ara(void* p) {
    VERI_TABANI& vt = (*(VERI_TABANI*)p);
    ICBYTES searchKey, temizKey;
    GetText(SLE, searchKey);
    NormalizeKey(searchKey, temizKey);
    unsigned long long* map = KeyMapTR(temizKey, 2);


    CreateMatrix(temizKey, 1, 0, 1, ICB_CHAR);
    CreateMatrix(temizKey, 1, 0, 1, ICB_CHAR);
    for (int i = 1; i <= searchKey.Y(); i++) {
        char c = searchKey.C(1, i);
        if (c != ' ') {
            if (c >= 'A' && c <= 'Z') c = c + 32; // büyük harfi küçüğe çevir
            temizKey += c;
        }
    }
    unsigned hangisi = IndexAra(vt.index, map);
    if (hangisi != 0xffffffff) {
        currentRecord = hangisi;
        DisplayRecord(p);
    }
    else {
        ICG_printf(MLE, "Record not found.\n");
    }

    ICG_printf(MLE, "Aranan temiz key: %s\n", &temizKey.C(1));
    ICG_printf(MLE, "Indexte %d kayıt var.\n", vt.index.Y());
    
}


void Onceki(void* p) {
    if (currentRecord > 1) {
        currentRecord--;
        DisplayRecord(p);
    }
    else {
        ICG_printf(MLE, "No previous record.\n");
    }
}

void Sonraki(void* p) {
    VERI_TABANI& vt = (*(VERI_TABANI*)p);
    if (currentRecord < vt.index.Y()) {
        currentRecord++;
        DisplayRecord(p);
    }
    else {
        ICG_printf(MLE, "No more records.\n");
    }
}

void Baslama(VERI_TABANI& v) {
    CreateMatrix(v.index, 3, 1, ICB_ULONGLONG);
    v.index = 0;
    CreateFileDevice(v.index_dosya, "index.bin");
    CreateFileDevice(v.veri_dosya, "veri.bin");
    ReadICBYTES(v.index_dosya, v.index, 0);
    ICG_printf(MLE, "Index loaded. Kayıt sayısı: %d \n", (v.index.Y())-1);

}

void ExitFonksiyonu(void* t) {
    VERI_TABANI& veritaban = (*(VERI_TABANI*)t);
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
        // İlk önce sadece ana çizgiyi çizer
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

    // Arka planın gösterilmesi
    ReadImage("gym2.bmp", arkaplan);
    ArkaPlanFrame = ICG_FrameThick(0, 0, 900, 600); 

    ICG_StaticPanel(550, 420, 270, 40, " Take a Step for a Fit and Healthy Life!");

    ICG_StaticPanel(400, 10, 150, 30, "         SIGN UP");

    // Name, Surname, Phone, Email, Password, Gender, Health, Health Description
    ICG_StaticPanel(50, 50, 80, 30, "Name:");
    NameInput = ICG_SLEditThick(150, 50, 200, 30, "");

    ICG_StaticPanel(50, 90, 80, 30, "Surname:");
    SurnameInput = ICG_SLEditThick(150, 90, 200, 30, "");

    ICG_StaticPanel(50, 130, 80, 30, "Phone:");
    PhoneInput = ICG_SLEditThick(150, 130, 200, 30, "");

    ICG_StaticPanel(50, 170, 80, 30, "E-mail:");
    EmailInput = ICG_SLEditThick(150, 170, 200, 30, "");

    ICG_StaticPanel(50, 210, 90, 30, "Password:");
    PasswordInput = ICG_SLPasswordSunken(150, 210, 200, 30);

    ICG_StaticPanel(50, 250, 80, 30, "Gender:");
    GenderInput = ICG_SLEditThick(150, 255, 150, 30, "");

    ICG_StaticPanel(50, 290, 220, 30, "Do you have health problems?");
    HealthProblemInput = ICG_SLEditThick(280, 295, 110, 30, "");

    ICG_StaticPanel(50, 335, 250, 30, "Health Problem Description (if any): ");
    ProblemInput = ICG_SLEditThick(310, 335, 200, 30, "");

    ICG_StaticPanel(50, 390, 250, 30, "Registration Status:");
    MLE = ICG_MLEditSunken(50, 420, 450, 80, "", SCROLLBAR_V);

    ICG_StaticPanel(550, 300, 75, 30, "Search:");
    SLE = ICG_SLEditThick(630, 300, 215, 30, "");

    ICG_Button(762, 340, 80, 30, "Search", Ara, &veritaban);
    ICG_Button(550, 340, 100, 30, "<< Previous", Onceki, &veritaban);
    ICG_Button(655, 340, 100, 30, "Next >>", Sonraki, &veritaban);

    // Register Button
    RegisterButton = ICG_Button(650, 475, 100, 30, "Register Now!", RegisterCustomer, nullptr);
    UploadPhotoButton = ICG_Button(645, 225, 100, 40, "Upload photo", UploadPhoto);

    // PhotoFrame
    CreateImage(PhotoPreview, 150, 150, 3, ICB_UCHAR);


    FillRect(PhotoPreview, 0, 0, 150, 150, 0xffffff); 

    int centerX = 75;  // Ortada X koordinatı
    int centerY = 75;  // Ortada Y koordinatı
    int radius = 35;   // Yarıçap
    int borderColor = 0x000000; // Siyah renk (çerçeve rengi)

    // Profil ikonu: Profil fotoğrafını temsil eden daire
    DrawFilledCircle(PhotoPreview, centerX, centerY, 35, 0x000000); //Daire

    int rectColor = 0x000000; // Siyah renk
    FillRect(PhotoPreview, centerX - 30, centerY + radius - 5, 60, 70, rectColor);  // Dikdörtgen

    // Fotoğraf çerçevesi
    PhotoFrame = ICG_FrameThick(610, 50, 150, 150);

    // Fotoğrafı çerçeveye yerleştir
    DisplayImage(PhotoFrame, PhotoPreview);

    DisplayImage(ArkaPlanFrame, arkaplan);
    ICG_SetOnExit(ExitFonksiyonu, &veritaban);
    Baslama(veritaban);

}