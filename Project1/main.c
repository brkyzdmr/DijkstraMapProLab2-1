#include <windows.h>
//#include <objidl.h>
#include <stdio.h>
#include <string.h>
#include <locale.h>

#define IDC_EXIT_BUTTON 1
#define IDC_KODBELGESIEKLE_FUNC 2
#define IDC_MESAFEBELGESIEKLE_FUNC 3
#define IDC_HARITABELGESIEKLE_FUNC 4
#define IDC_HESAPLA_FUNC 5
#define IDC_BASLANGICTEXT 6
#define IDC_GIDILECEKTEXT 7
#define SONSUZ_UZAKLIK 0x7FFFFFFF

HINSTANCE hInst;
HDC hdc;
int Width = 1366, Height = 768;     // 1366x768

BOOL haritaNokt = FALSE;
char *sehirler[MAX_PATH];       // Harita noktalari olan sehirler
char *kBSehirler[MAX_PATH];
char *mBSehirler[MAX_PATH][2];
int *mesafe[MAX_PATH];
int koordinatXY[90][2];
char line[128];
char *deger;
int sayi;
int sehirSayisi;
int komsulukBoyut;
char* kodBelgesiYolu;
TCHAR baslangicText[20];
TCHAR gidilecekText[20];
TCHAR sehir[20];
int baslangicIndeks;
int gidilecekIndeks;
char ROTA[81][81];
int maliyet;

typedef struct Sehir{
    char sehir[20];
    int x;
    int y;
} x;

LRESULT CALLBACK WindProcedure(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);

INT WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                   LPSTR lpCmdLine, int nCmdShow)
{
    setlocale(LC_ALL,"Turkish");    // Consol ekranına Türkçe (ç,ş,ğ,ı,ü,ö) karakterlerin yazdırılabilmesi için kullanılır.

    WNDCLASSEX  WndCls;
    static char szAppName[] = "Brkyzdmr";
    MSG         Msg;

    // Uygulama ayarları bölümü
	hInst       = hInstance;
    WndCls.cbSize        = sizeof(WndCls);
    WndCls.style         = CS_OWNDC | CS_VREDRAW | CS_HREDRAW;
    WndCls.lpfnWndProc   = WindProcedure;
    WndCls.cbClsExtra    = 0;
    WndCls.cbWndExtra    = 0;
    WndCls.hInstance     = hInst;
    WndCls.hIcon         = LoadIcon(NULL, IDI_APPLICATION);
    WndCls.hCursor       = LoadCursor(NULL, IDC_ARROW);
    WndCls.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
    WndCls.lpszMenuName  = NULL;
    WndCls.lpszClassName = szAppName;
    WndCls.hIconSm       = LoadIcon(hInstance, IDI_APPLICATION);
    RegisterClassEx(&WndCls);

    // Ekranın oluşturulma bölümü
    CreateWindowEx(WS_EX_OVERLAPPEDWINDOW,
                          szAppName,
                          "Berkay Ezdemir 150202041",
                          WS_POPUP | WS_VISIBLE,
                          CW_USEDEFAULT,
                          CW_USEDEFAULT,
                          Width,
                          Height,
                          NULL,
                          NULL,
                          hInstance,
                          NULL);

    while( GetMessage(&Msg, NULL, 0, 0) )
    {
        TranslateMessage(&Msg);
        DispatchMessage( &Msg);
    }

    return 0;
}

LRESULT CALLBACK WindProcedure(HWND hWnd, UINT Msg,
			   WPARAM wParam, LPARAM lParam)
{

    static HBITMAP hBitmap;
    PAINTSTRUCT Ps;
    HFONT headlineFont;
    HFONT normFont;
    LOGFONT lfont;
    BITMAP bitmap = {0};
    HDC hdcMem;
    HGDIOBJ oldBitmap;

    switch(Msg)
    {
        case WM_DESTROY:
            // Oluşturulan nesnelerin kullanımı bittikten sonra yok edildiği bölüm

            DeleteObject(oldBitmap);
            DeleteObject(hBitmap);
            PostQuitMessage(WM_QUIT);
            break;

        case WM_CREATE:
            // WinApi için oluşturma bölümü, kullanılacak herşey burada oluşturulur.

            hBitmap = (HBITMAP) LoadImageW(NULL, L"C:\\Users\\berka\\Desktop\\ProLab2 - 1\\3.bmp",
                        IMAGE_BITMAP, Width, Height, LR_LOADFROMFILE);

            HWND hwBtn_Exit = CreateWindow(TEXT("BUTTON"), TEXT("EXIT"),
                                WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
                                1306, 10, 50, 50,
                                hWnd, (HMENU) IDC_EXIT_BUTTON, NULL, NULL);

// Kod belgesi ekle butonu

            HWND hwBtnKBEkle = CreateWindow(TEXT("BUTTON"), TEXT("Kod Belgesi Ekle"),
                                            WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
                                            30, 115, 150, 50,
                                            hWnd, (HMENU) IDC_KODBELGESIEKLE_FUNC, NULL, NULL);

// Mesafe belgesi ekle butonu

            HWND hwBtnMBEkle = CreateWindow(TEXT("BUTTON"), TEXT("Mesafe Belgesi Ekle"),
                                            WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
                                            30, 115+60, 150, 50,
                                            hWnd, (HMENU) IDC_MESAFEBELGESIEKLE_FUNC, NULL, NULL);

// Harita belgesi ekle butonu

            HWND hwBtnHBEkle = CreateWindow(TEXT("BUTTON"), TEXT("Harita Belgesi Ekle"),
                                            WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
                                            30, 175+60, 150, 50,
                                            hWnd, (HMENU) IDC_HARITABELGESIEKLE_FUNC, NULL, NULL);

// Baslangic sehri texbox

            HWND hwTxbBaslangic = CreateWindow(TEXT("EDIT"), TEXT(""),
                                            WS_VISIBLE | WS_CHILD | WS_BORDER | ES_AUTOHSCROLL,
                                            30, 375, 150, 25,
                                            hWnd, (HMENU) IDC_BASLANGICTEXT, NULL, NULL);

// Gidilecek sehir texbox

            HWND hwTxbGidilecek = CreateWindow(TEXT("EDIT"), TEXT(""),
                                            WS_VISIBLE | WS_CHILD | WS_BORDER | ES_AUTOHSCROLL,
                                            30, 450, 150, 25,
                                            hWnd, (HMENU) IDC_GIDILECEKTEXT, NULL, NULL);

// Hesapla butonu

            HWND hwBtnHesapla = CreateWindow(TEXT("BUTTON"), TEXT("HESAPLA"),
                                            WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
                                            30, 500, 150, 50,
                                            hWnd, (HMENU) IDC_HESAPLA_FUNC, NULL, NULL);


            break;

        case WM_PAINT:
            // Ekrana çizim yaptırılan bölüm
             hdc = BeginPaint(hWnd, &Ps);

             RECT rect;
             LPPOINT pt;
             LOGPEN LogPen;
             HPEN redPen;
             HPEN blackPen;
             HBRUSH newBrush;

             hdcMem = CreateCompatibleDC(hdc);
             oldBitmap = SelectObject(hdcMem, hBitmap);

             GetObject(hBitmap, sizeof(bitmap), &bitmap);
             LONG cx = bitmap.bmWidth;
             LONG cy = bitmap.bmHeight;
             BitBlt(hdc, 0, 0, cx, cy,
                 hdcMem, 0, 0, SRCCOPY);

             SelectObject(hdcMem, oldBitmap);

            // Yeni yazı fontları oluşturuluyor.
            headlineFont = CreateFont(40, 20, 0, 0,
                           FW_NORMAL, FALSE, FALSE, FALSE,
                           ANSI_CHARSET, OUT_DEFAULT_PRECIS,
		         CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
		         PROOF_QUALITY | FF_ROMAN,
			"Apple Boy BTN");

			normFont = CreateFont(18, 9, 0, 0,
                           FW_NORMAL, FALSE, FALSE, FALSE,
                           ANSI_CHARSET, OUT_DEFAULT_PRECIS,
		         CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
		         PROOF_QUALITY | FF_ROMAN,
			"Apple Boy BTN");


            //LogPen.lopnColor = RGB(255,0,0);

           // hPen = CreatePenIndirect(&LogPen);

/*
            lfont.lfStrikeOut = 0;
            lfont.lfUnderline = 0;
            lfont.lfHeight = 50;
            lfont.lfEscapement = 0;
            lfont.lfItalic = TRUE;
            font = CreateFontIndirect(&lfont);
*/
            SelectObject(hdc, normFont);
            SetTextColor(hdc, RGB(0,0,0));

            TextOut(hdc, 30, 350, "Baslangic Sehri :", 17);
            TextOut(hdc, 30, 425, "Gidilecek Sehir :", 17);

            SelectObject(hdc, headlineFont);
            SetTextAlign(hdc, TA_CENTER);
            TextOut(hdc, 750, 25, "SEHIRLERARASI KISA MESAFE CETVELI", 34);

            //SetTextAlign(hdc, TA_BASELINE);



            DeleteObject(blackPen);
            DeleteObject(redPen);
            DeleteObject(hdcMem);
            DeleteObject(headlineFont);
            DeleteObject(normFont);

            EndPaint(hWnd, &Ps);
            break;

        case WM_KEYDOWN:
            // Klavye kısayolları bölümü
            if(wParam == 'S') {

                HaritaNoktalari();

            } else if(wParam == 'B') {

                MessageBox(hWnd, "Deneme kisayol tusu basildi!", "Mesaj!", MB_OK);
            }

            break;

            case WM_COMMAND:
                // İşaretlenen makrolara göre işlem oluşturma bölümü

                switch(LOWORD(wParam))
                {
                    int statB = 0;
                    int statG = 0;

                    case IDC_EXIT_BUTTON:

                        SendMessage(hWnd, WM_CLOSE, 0, 0);

                        break;

                    case IDC_KODBELGESIEKLE_FUNC:

                        BelgeEkle(1);

                        printf(kodBelgesiYolu);

                        break;

                    case IDC_MESAFEBELGESIEKLE_FUNC:

                        BelgeEkle(2);

                        printf(kodBelgesiYolu);

                        break;

                    case IDC_HESAPLA_FUNC:
                        // Hesapla butonuna basıldığında buradaki işlemler gerçekleşir.

                        statB = GetWindowText(GetDlgItem(hWnd, IDC_BASLANGICTEXT), baslangicText, 20);
                        statG = GetWindowText(GetDlgItem(hWnd, IDC_GIDILECEKTEXT), gidilecekText, 20);

                        if((statB != 0) && (statG != 0)) {
                            KaynakVeHedefSehir();
                            KomsulukMatrisiOlustur();

                            //MessageBox(hWnd, "Basarili", "Mesaj!", MB_OK);
                            printf("%s -> %s \n", baslangicText, gidilecekText);

                            printf("Rota : %s, Maliyet : %d\n", ROTA[gidilecekIndeks], maliyet);
                            //MessageBox(hWnd, ROTA[gidilecekIndeks], maliyet, MB_OK);
                            EkranaBastir();
                            HaritaNoktalari();

                        }

                        break;

                    case IDC_HARITABELGESIEKLE_FUNC:

                        BelgeEkle(3);

                        break;

                }
        default:
            return DefWindowProc(hWnd, Msg, wParam, lParam);
    }
    return 0;
}

void HaritaNoktalari() {
    /*  Bu fonksiyon kod belgesindeki bulunan şehirlere göre
    *   harita belgesi kullanılarak bulunan şehrin karşılaştırılması
    *   ile koordinatlar kullanılarak ekrandaki haritaya şehirlerin
    *   merkez noktalarına dikdörtgen eklenmesi işlemini yapmaktadır.
    *   Hedef ve kaynak şehirler kırmızı dikdörtgenler şeklinde
    *   oluşturulmaktadır.
    */

    int i,j, size=0;
    HPEN redPen;
    HPEN blackPen;

    redPen = CreatePen(PS_SOLID, 5, RGB(255,0,0));
    blackPen = CreatePen(PS_SOLID, 1, RGB(0,0,0));

    SelectObject(hdc, blackPen);
    Rectangle(hdc, 0, 0, 10, 10);

   for(i=1; i<sehirSayisi; i++) {
        for(j=1; j<82; j++) {
            if(strcmp(kBSehirler[i], sehirler[j]) == 0) {
                Rectangle(hdc, koordinatXY[j][0],koordinatXY[j][1],
                  koordinatXY[j][0]+20,koordinatXY[j][1]+20);
            }
        }
    }
   for(j=0; j<82; j++) {
        if(strcmp(baslangicText, sehirler[j]) == 0){
            SelectObject(hdc, redPen);
            Rectangle(hdc, koordinatXY[j][0],koordinatXY[j][1],
              koordinatXY[j][0]+20,koordinatXY[j][1]+20);

              printf("\nindex: %d\n",j);
              break;
        }
    }
    printf("\nSehir isimlerini iceren dizilerin karsilastirmasinda sorun yasandigi icin\nsonuc yalnizca konsolda gorulmektedir.\n");

}

void NoktalariSil() {
    // Nesne silme fonksiyonu deneme1

    int i,j;
    HPEN redPen;
    HPEN blackPen;

    DeleteObject(redPen);
    DeleteObject(blackPen);
}

void BelgeEkle(int secim) {
    /*  Bu fonksiyon her 3 belge ekle fonksiyonu
    *   için diyalog ekranı oluşturulması ve bu
    *   ekrandan seçilen belge yolunun gerekli
    *   fonksiyonlara gönderilmesi işlemini
    *   gerçekleştirmektedir.
    */
    system("CLS");

    OPENFILENAME ofn;
    char dosyaYolu[] = "";
    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(OPENFILENAME);
    ofn.hwndOwner = NULL;
    ofn.lpstrFilter = "Text Files (*.txt*)\0*.txt*\0";
    ofn.nFilterIndex = 1;
    ofn.lpstrFile = dosyaYolu;
    ofn.nMaxFile = MAX_PATH;
    ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
    ofn.lpstrDefExt = "";

    //GetSaveFileName(&ofn);
    GetOpenFileName(&ofn);

    FILE *file;

    if((file = fopen(dosyaYolu, "r+")) == NULL) {
         MessageBox(NULL, TEXT("Dosya Eklenemedi!"), TEXT("UYARI!"), MB_ICONEXCLAMATION);
    } else {
        if(secim == 1) {
           // printf("1\n");
            KodBelgesiBuffer(dosyaYolu);
        } else if(secim == 2) {
           // printf("2\n");
            MesafeBelgesiBuffer(dosyaYolu);
        } else if(secim == 3) {
           // printf("3\n");
            KoordinatBuffer(dosyaYolu);
        }
    }
}

void AdresYazdir(char* dosyaYolu) {
    // Dosya yolunu ekrana yazdırır.
     printf(dosyaYolu);
}

void KoordinatBuffer(char* dosyaYolu) {
    /*  Harita belgesi ekle butonuna basıldığında
    *   çalıştırılacak olan fonksiyondur. Bu fonksiyonda
    *   açılan dosyadaki şehir-x,y formatına
    *   uyularak dosyadan şehrin kesilip bir diziye,
    *   koordinatların da kesilerek ayrı bir matrise atılma
    *   işlemi yapılmaktadır.
    */

    FILE *dosya = fopen ( dosyaYolu, "r" );

    int i=0;

    while(fgets(line, sizeof(line), dosya)){

        deger = strtok(line, "-");
        sehirler[i] = strdup(deger);
        //sehirler[i] = strtok(line, "-");

        deger = strtok(NULL, ",");
       // sayi = atoi(deger);
        koordinatXY[i][0] =  atoi(deger);

        deger = strtok(NULL, "\n");
       // sayi = atoi(deger);
        koordinatXY[i][1] =  atoi(deger);

        i++;
    }

    //DiziyiYazdir();


    fclose(dosya);
}

void KodBelgesiBuffer(char* dosyaYolu) {
    /*  Kod belgesi ekle butonuna basıldığında
    *   çalıştırılacak olan fonksiyondur. Bu fonksiyonda
    *   açılan dosyadaki şehir1-şehirno formatına
    *   uyularak dosyadan şehrin kesilip bir diziye atılma
    *   işlemi yapılmaktadır.
    */

    FILE *dosya = fopen ( dosyaYolu, "r" );

    int i=0;

    while(fgets(line, sizeof(line), dosya)){

        deger = strtok(line, "-");
        kBSehirler[i] = strdup(deger);
        //kBSehirler[i] = strtok(line, "-");

        i++;
    }

    sehirSayisiVer(i);

   // KBBufferYazdir();


    fclose(dosya);
}

void sehirSayisiVer(int sayi) {
    sehirSayisi = sayi-1;
    printf("\nSehir Sayisi = %d\n", sehirSayisi);
    komsulukBoyut = sehirSayisi;
}

void MesafeBelgesiBuffer(char* dosyaYolu) {
    /*  Mesafe belgesi ekle butonuna basıldığında
    *   çalıştırılacak olan fonksiyondur. Bu fonksiyonda
    *   açılan dosyadaki şehir1-şehir2-mesafe formatına
    *   uyularak dosyadan şehirlerin kesilip bir matrise,
    *   mesafelerin de kesilerek ayrı bir diziye atılma
    *   işlemi yapılmaktadır.
    */
    FILE *dosya = fopen ( dosyaYolu, "r" );

    int i=0;

    while(fgets(line, sizeof(line), dosya)){

        deger = strtok(line, "-");
        mBSehirler[i][0] = strdup(deger);

        deger = strtok(NULL, "-");
        mBSehirler[i][1] = strdup(deger);

        deger = strtok(NULL, " \n");
        mesafe[i] = atoi(deger);

        i++;
    }



   // MBBufferYazdir();


}

void MBBufferYazdir() {
    int k,j;

    for(k=1; (mBSehirler[k][0] != NULL); k++){
        for(j=0;j<2;j++){
            printf("%s ", mBSehirler[k][j]);
        }
        printf("%d\n", mesafe[k]);
    }
}

void KBBufferYazdir() {
    int i;

    // (kBSehirler[i] == NULL)
    for(i=1; (kBSehirler[i] != NULL); i++){
        printf("%s\n", kBSehirler[i]);
    }

}

void DiziyiYazdir() {
    int k, j;

    for(k=1; k<82; k++){
        for(j=0;j<2;j++){
           printf("%d, ", koordinatXY[k][j]);
        }
        printf("%s\n", sehirler[k]);
    }
}

void KomsulukMatrisiOlustur() {
    /*  Komşuluk matrisi verilen şehir kod belgesindeki şehirler
    *   kullanılarak oluşturulan simetrik matrisin karayollarının
    *   iller arası mesafe cetveli örneğinde de görüldüğü üzere
    *   matristeki şehirlerin birbirlerine olan uzaklıkları örüntüsel
    *   bir biçimde ilerlediği için aşağıdaki algoritma ile yazılmıştır.
    */
    int i, j, k, h;
    int komsulukMatrisi[komsulukBoyut][komsulukBoyut];

    int sehirNo1 = 0;
    int sehirNo2 = 0;
    int ilkSehir = 0;
    int ikinciSehir = 0;

    // Komsuluk matrisine ilk olarak tum mesafeler sonsuz olarak giriliyor.
    for(i=0; i<komsulukBoyut; i++) {
        for(j=0; j<komsulukBoyut; j++) {
            komsulukMatrisi[i][j] = -1;
        }
    }

    // Komsuluk matrisinin olusturulmasi
    for(i=1; mBSehirler[i][0] != NULL; i++) {
        for(j=1; kBSehirler[j] != NULL; j++) {
                if(!strcmp(mBSehirler[i][0], kBSehirler[j])) {
                    ilkSehir = 1;
                    sehirNo1 = j;
                }

                if(!strcmp(mBSehirler[i][1], kBSehirler[j])) {
                    ikinciSehir = 1;
                    sehirNo2 = j;
                }

                if(ilkSehir == 1 && ikinciSehir == 1) {
                    komsulukMatrisi[sehirNo1-1][sehirNo2-1] = mesafe[i];
                    komsulukMatrisi[sehirNo2-1][sehirNo1-1] = mesafe[i];
                    ilkSehir = 0;
                    ikinciSehir = 0;
                }
        }

    }

   //KomsulukMatrisiYazdir(komsulukMatrisi);

    // Dijkstra
   Dijkstra(komsulukMatrisi, baslangicIndeks, gidilecekIndeks);


}

void KaynakVeHedefSehir() {
    int i;

    for(i=0; kBSehirler[i] !=  NULL; i++) {
            if(!strcmp(baslangicText, kBSehirler[i])) {
                baslangicIndeks = i-1;
                printf("Baslangic : %d\n", baslangicIndeks);
            }
            if(!strcmp(gidilecekText, kBSehirler[i])) {
                gidilecekIndeks = i-1;
                printf("Gidilecek : %d\n", gidilecekIndeks);
            }
    }
}

void Dijkstra(int komsulukMatrisi[komsulukBoyut][komsulukBoyut], int kaynak, int hedef) {
    /*  Dijkstra algoritması Rifat Çölkesen - Veri Yapıları ve Algoritmalar
    *   kitabındaki örnek kullanarak gerçekleştirildi.
    */
    char ELEALINDI[komsulukBoyut];
    char *ptr;
    int i, j, ead, ek;
    int EKM[komsulukBoyut];
  //  char ROTA[komsulukBoyut][komsulukBoyut];

    for(i=0; i<komsulukBoyut; i++) {
        EKM[i] = SONSUZ_UZAKLIK;
        ELEALINDI[i] = 0;
    }

    EKM[kaynak] = 0;
    ead = kaynak;

    for(i=0; i<komsulukBoyut; i++) {
        for(j=0; j<komsulukBoyut; j++)
            if(!ELEALINDI[j])
            if(komsulukMatrisi[ead][j] != -1)
            if(EKM[j] > komsulukMatrisi[ead][j] + EKM[ead]) {
                EKM[j] = komsulukMatrisi[ead][j] + EKM[ead];
                strcpy(ROTA[j], ROTA[ead]);
                ptr = ROTA[j];

                while(*ptr != NULL)
                    ptr++;
                *ptr = 'A' + ead;
            }
            ek = SONSUZ_UZAKLIK;

            for(j=0; j<komsulukBoyut; j++)
                if(!ELEALINDI[j])
                if(EKM[j] < ek) {
                    ek = EKM[j];
                    ead = j;
                }
                ELEALINDI[ead] = 1;
    }


   // Sonuc(EKM, ROTA);
    Sonuc(EKM);
}

void Sonuc(int Maliyet[komsulukBoyut]) {
    /*  Dijkstra fonksiyonunda bulunan değerlerin
    *   ekrana yazdırılma işlemini gerçekleştirmektedir.
    */
    int i, j;

    printf("\nSehir Kodu______Sehir Adi__________Mesafe__________Rota\n");
    for(i=0;i<sehirSayisi; i++) {
        if(Maliyet[i] == 0) {
            printf("%c :\t%15s\t     =\t   Baslangic Sehri\n", 'A'+i, kBSehirler[i+1]);
        } else {
            printf("%c :\t%15s\t     =\t   %d\t\t   %s\n", ('A'+i), kBSehirler[i+1], Maliyet[i], ROTA[i]);
        }
    }

    printf("\n");

    maliyet = Maliyet[gidilecekIndeks];

}

void EkranaBastir() {
    TCHAR buff[30];

    HFONT resultFont = CreateFont(18, 9, 0, 0,
                           FW_NORMAL, FALSE, FALSE, FALSE,
                           ANSI_CHARSET, OUT_DEFAULT_PRECIS,
		         CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
		         PROOF_QUALITY | FF_ROMAN,
			"Apple Boy BTN");

    SelectObject(hdc, resultFont);
    SetTextColor(hdc, RGB(255, 0, 0));
    SetTextAlign(hdc, TA_CENTER);
    TextOut(hdc, 1366/2, 300, buff, wsprintf(buff, "%s > %s : %d KM", kBSehirler[baslangicIndeks+1],
                                         kBSehirler[gidilecekIndeks+1], maliyet));


}

void KomsulukMatrisiYazdir(int komsulukMatrisi[komsulukBoyut][komsulukBoyut]) {
    int i,j;

    for(i=0; i<komsulukBoyut; i++) {
        for(j=0; j<komsulukBoyut; j++) {
            printf("%d ", komsulukMatrisi[i][j]);
        }
        printf("\n");
    }
}
