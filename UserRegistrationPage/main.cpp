#include "icb_gui.h"
#include "ic_media.h"
#include "icbytes.h"

int MLE; // Çok satýrlý metin alaný
int NameInput, SurnameInput, PhoneInput, EmailInput, PasswordInput;
int RegisterButton, UploadPhotoButton, PhotoFrame;

ICBYTES dosyaYolu, foto, kucultulmus, sonFoto;
int FRM;

struct VERI_TABANI {
    ICDEVICE index_dosya, veri_dosya;
    ICBYTES index, anahtar, bilgi;
    unsigned burayabak = 1;
};

void ICGUI_Create() {
    ICG_MWSize(600, 500);
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
    char* yol = OpenFileMenu(dosyaYolu, "JPG\0*.JPG\0PNG\0*.PNG\0BMP\0*.BMP\0JPEG\0*.JPEG");
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
    ICBYTES ad, soyad, telefon, email, sifre, bilgi, virgul, newline;
    virgul = ","; newline = "\n";

    ad = ""; soyad = ""; telefon = ""; email = ""; sifre = "";
    veritaban.anahtar = ""; veritaban.bilgi = "";

    GetText(NameInput, ad);
    GetText(SurnameInput, soyad);
    GetText(PhoneInput, telefon);
    GetText(EmailInput, email);
    GetText(PasswordInput, sifre);

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
