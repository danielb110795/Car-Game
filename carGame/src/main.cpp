#include <iostream>
#include <fstream>
#include <windows.h>
#include <vector>
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_native_dialog.h>

using namespace std;

struct ruch
{
    double x;
    double y;
} samochod, przeciwnik;

bool initAllegro()
{
    if(!al_init())                          //inicjalizacja Allegro
        return false;
    if(!al_install_keyboard())              // instalacja klawiatury
        return false;
    if(!al_init_image_addon())              //inicjalizacja obs³ugi grafiki
        return false;
    if(!al_init_primitives_addon())         // inicjalizacja modu³u rysowania kszta³tów
        return false;
    if(!al_install_audio())                 //instalacja obs³ugi dŸwiêku
        return false;
    if(!al_init_acodec_addon())             //inicjalizacja dŸwiêku
        return false;
    if(!al_reserve_samples(2))              //zarezerwowanie mo¿liwoœci za³adowania 3 sampli
        return false;
    if(!al_init_ttf_addon())                //zarezerwowanie mo¿liwoœci za³adowania 3 sampli
        return false;
    return true;                            //jeœli funkcja dota³a do tego miejsca zwraca prawdê
}

int readhighscore ()
{
    ifstream rekord;
    rekord.open( "rekord.txt" );
    if( !rekord.good() )
        return false;
    int high;
    rekord >> high;
    rekord.close();
    return high;
}

bool savehighscore (int score)
{
    ifstream rekord;
    rekord.open( "rekord.txt" );
    if( !rekord.good() )
        return false;
    int zpliku;
    rekord >> zpliku;
    rekord.close();
    if (zpliku < score)
    {
        ofstream rekord;
        rekord.open( "rekord.txt", ios::out);
        rekord << score;
        rekord.close();
    }
    return true;
}

int showOptionsList(ALLEGRO_DISPLAY *window, const char *options[], int nOptions, ALLEGRO_FONT *font, int x, int y, int width)
{
    int selected = 0;                                                                               //aktualnie wybrana pozycja z listy
    ALLEGRO_KEYBOARD_STATE keyboard;                                                                //stan klawiatury
    ALLEGRO_BITMAP *background = al_clone_bitmap(al_get_backbuffer(window));                        //skopiowanie t³¹

    int textHeight = al_get_font_line_height(font);                                                 //pobranie wysokoœci tekstu

    int i;
    double time = al_get_time();                                                                    //pobranie aktualnego czasu

    do
    {
        al_draw_bitmap(background, 0, 0, 0);                                                        //rysowanie t³a
        for(i=0; i<nOptions; ++i)                                                                   //przeœcie przez wszystkie opcje
        {
            if(i == selected)                                                                       //jeœli aktualna opcja jest zaznaczona, rysowane jest pod ni¹ t³o
                al_draw_filled_rectangle(x + 40, y + (textHeight * i), x + width - 40, y +(textHeight * (i + 1)), al_map_rgb(189, 199, 126));
            al_draw_text(font, al_map_rgb(34, 77, 93), (width / 2) + x, y + (textHeight * i), ALLEGRO_ALIGN_CENTER, options[i]);
        }                                                                                           //wypisanie opcji
        al_flip_display();                                                                          //odœwie¿enie obrazu

        al_get_keyboard_state(&keyboard);                                                           //pobranie stanu klawiatury
        if(al_get_time() > time + 0.1)                                                              //jeœli up³ynê³a odpowiedni iloœc czasu od ostatniej zmiany zaznaczenia
        {
            if(al_key_down(&keyboard, ALLEGRO_KEY_DOWN))                                            //jeœli wciœniêto strza³kê w dó³
            {
                selected++;                                                                         //zaznaczenie zmienia siê na pozycjê nastêpn¹
                if(selected == nOptions)                                                            //jeœli zanaczona by³a ostatnia pozycja na liœcie
                    selected = 0;                                                                   //zaznacznie pierwszej pozycji na liœcie
            }
            else if(al_key_down(&keyboard, ALLEGRO_KEY_UP))                                         //jeœli wciœniêto strza³kê do góry
            {
                selected--;                                                                         //zaznaczenie poprzedniej opcji
                if(selected < 0)                                                                    //jeœli zaznaczona by³a pierwsza opcja
                    selected = nOptions - 1;                                                        //zaznaczenie ostatniej opcji menu
            }
            time = al_get_time();
        }
    }
    while(!al_key_down(&keyboard, ALLEGRO_KEY_ENTER));                                              //pêtla powtarzana jest do czasu wciœniêcia ENTER (zatwierdzenia wyboru)
    al_destroy_bitmap(background);                                                                  //zwolnienie pamiêci
    return selected;                                                                                //zwrócenie wybranej opcji
}

int showPauseMenu(ALLEGRO_DISPLAY *window, ALLEGRO_FONT *fontText, ALLEGRO_FONT *fontScore, int score)
{
    int width = al_get_display_width(window);                                                       //szerokoœæ okna
    int height = al_get_display_height(window);                                                     //wysokoœæ okna

    int menuWidth = width / 2;                                                                      //szerokoœæ menu
    int menuHeight = height / 2;                                                                    //wysokoœc menu
    int menuPositionX = width / 4;                                                                  //pozycja X menu
    int menuPositionY = height / 4;                                                                 //pozycja Y menu

    ALLEGRO_BITMAP *background = al_clone_bitmap(al_get_backbuffer(window));                        //zapisanie obrazu przedstawiaj¹cego aktualny wygl¹d okna

    int i;
    for(i=0; i<=menuHeight; ++i)                                                                    //pêtla rozwijaj¹ca menu
    {
        al_draw_bitmap(background, 0, 0, 0);                                                        //rysowanie t³a
        al_draw_filled_rounded_rectangle(menuPositionX, menuPositionY, menuPositionX + menuWidth, menuPositionY + i, 20, 20, al_map_rgba(140, 255, 200, 100));
        //rysowanie t³a menu
        al_flip_display();                                                                          //odœwie¿enie obrazu
    }

    char textScore[10];                                                                             //tekst zawieraj¹cy informacjê o wyniku gracza
    //tekst zawieraj¹cy informacjê o aktualnym poziomie
    sprintf(textScore, "%d", score);


    int textHeight = al_get_font_line_height(fontText);                                             //pobranie wysokoœci tekstu
    //----Wypisanie tektu w menu:
    al_draw_text(fontText, al_map_rgb(34, 77, 93), width / 2, 10 + menuPositionY, ALLEGRO_ALIGN_CENTER, "PAUZA");
    al_draw_text(fontText, al_map_rgb(34, 77, 93), menuPositionX + 10, 10 + menuPositionY + textHeight, ALLEGRO_ALIGN_LEFT, "Aktualny wynik:");
    al_draw_text(fontScore, al_map_rgb(34, 77, 93), menuPositionX + menuWidth - 10, 10 + menuPositionY + textHeight, ALLEGRO_ALIGN_RIGHT, textScore);

    al_flip_display();                                                                              //odœwie¿enie obrazu

    int result;                                                                                     //zmienna przechowuj¹ca wybran¹ przez u¿ytkownika opcjê menu
    const char *options[3];                                                                         //tablica z opcjami do wyœwietlenia
    options[0] = "Kontynuuj";                                                                       //zapisanie opcji do tablicy
    options[1] = "Restart";
    options[2] = "Zamknij";
    result = showOptionsList(window, options, 3, fontText, menuPositionX, (menuPositionY + menuHeight) - 10 - (3 * textHeight), menuWidth);
    //pobranie wyboru gracza

    for(i=menuHeight; i>30; --i)                                                                    //pêtla zwijaj¹ca menu
    {
        al_draw_bitmap(background, 0, 0, 0);                                                        //wyœwietlenie t³a
        al_draw_filled_rounded_rectangle(menuPositionX, menuPositionY, menuPositionX + menuWidth, menuPositionY + i, 20, 20, al_map_rgba(140, 255, 200, 100));
        //wyœwietlenie t³a menu
        al_flip_display();                                                                          //odœwie¿enie obrazu
    }

    al_destroy_bitmap(background);                                                                  //zwolnienie pamiêci bitmapy
    return result;                                                                                  //zwrócenie wyboru gracza
}

int showNewGameMenu(ALLEGRO_DISPLAY *window,ALLEGRO_FONT *fontText, ALLEGRO_FONT *fontScore,int score)
{
    int width = al_get_display_width(window);                                                       //szerokoœc ekranu
    int height = al_get_display_height(window);                                                     //wysokoœæ ekranu

    int menuWidth = width / 2;                                                                      //szerokoœæ menu
    int menuHeight = height / 2;                                                                    //wysokoœæ menu
    int menuPositionX = width / 4;                                                                  //pozycja X menu
    int menuPositionY = height / 4;                                                                 //pozycja Y menu

    ALLEGRO_BITMAP *background = al_clone_bitmap(al_get_backbuffer(window));                        //aktualna zawartoœæ okna

    int i;
    for(i=0; i<=menuHeight; ++i)                                                                    //pêtla roziwjaj¹ca menu
    {
        al_draw_bitmap(background, 0, 0, 0);
        al_draw_filled_rounded_rectangle(menuPositionX, menuPositionY, menuPositionX + menuWidth, menuPositionY + i, 20, 20, al_map_rgba(140, 255, 200, 100));
        al_flip_display();
    }

    char textScore[10];                                                                             //aktualny wynik
    //aktualny poziom
    sprintf(textScore, "%d", score);

    int textHeight = al_get_font_line_height(fontText);                                             //wysokoœæ tekstu
    //---wypisanie tekstu manu:
    al_draw_text(fontText, al_map_rgb(34, 77, 93), width / 2, 10 + menuPositionY, ALLEGRO_ALIGN_CENTER, "KONIEC GRY");
    al_draw_text(fontText, al_map_rgb(34, 77, 93), menuPositionX + 10, 10 + menuPositionY + textHeight, ALLEGRO_ALIGN_LEFT, "Wynik:");
    al_draw_text(fontScore, al_map_rgb(34, 77, 93), menuPositionX + menuWidth - 10, 10 + menuPositionY + textHeight, ALLEGRO_ALIGN_RIGHT, textScore);
    al_flip_display();                                                                              //odœwie¿enie obrazu

    int result;
    const char *options[2];                                                                         //lista opcji
    options[0] = "Nowa gra";
    options[1] = "Zamknij";
    result = showOptionsList(window, options, 2, fontText, menuPositionX, (menuPositionY + menuHeight) - 10 - (2 * textHeight), menuWidth);
    //pobranie wyboru u¿ytkownika

    for(i=menuHeight; i>30; --i)                                                                    //pêtla zwijaj¹ca menu
    {
        al_draw_bitmap(background, 0, 0, 0);
        al_draw_filled_rounded_rectangle(menuPositionX, menuPositionY, menuPositionX + menuWidth, menuPositionY + i, 20, 20, al_map_rgba(140, 255, 200, 100));
        al_flip_display();
    }

    al_destroy_bitmap(background);                                                                  //zwolnienie pamiêci
    return result;
}

void drawScoreBoard(int score, int high, int level, ALLEGRO_FONT *fontText, ALLEGRO_FONT *fontScore)
{
    char textScore[10];
    char highscore[10];
    char textlevel[10];                                                                           //wynik gracza
    sprintf(textScore, "%d", score);
    sprintf(highscore, "%d", high);
    sprintf(textlevel, "%d", level);

    int textHeight = al_get_font_line_height(fontText);

    al_draw_filled_rounded_rectangle(684, 14, 794, 175, 20, 20, al_map_rgba(145, 145, 119, 255));
    al_draw_filled_rounded_rectangle(680, 10, 790, 171, 20, 20, al_map_rgba(189, 199, 126, 255));

    al_draw_text(fontText, al_map_rgb(34, 77, 93), 685, 25, ALLEGRO_ALIGN_LEFT, "Wynik:");
    al_draw_text(fontScore, al_map_rgb(34, 77, 93), 780, 30, ALLEGRO_ALIGN_RIGHT, textScore);
    al_draw_text(fontText, al_map_rgb(34, 77, 93), 685, 25 + textHeight, ALLEGRO_ALIGN_LEFT, "Level:");
    al_draw_text(fontScore, al_map_rgb(34, 77, 93), 780, 30 + textHeight, ALLEGRO_ALIGN_RIGHT, textlevel);
    al_draw_text(fontText, al_map_rgb(34, 77, 93), 685, 25 + textHeight * 2, ALLEGRO_ALIGN_LEFT, "Rekord:");
    al_draw_text(fontScore, al_map_rgb(34, 77, 93), 780, 30 + textHeight * 2, ALLEGRO_ALIGN_RIGHT, highscore);

}

class klasa_przeciwnik
{
public:

    float posX;
    float posY;
    int r, g, b;
    double czas1 = al_get_time();
    void init()
    {
        posY = -170;
        posX = 135 + ((rand()%520)/80)* 80;
        r = rand()%255;
        g = rand()%255;
        b = rand()%255;
    }

    void update(ALLEGRO_BITMAP *tekstura, double szybkosc_przeciwnikow)
    {
        al_draw_tinted_bitmap(tekstura,al_map_rgb (r, g, b), przeciwnik.x, przeciwnik.y, ALLEGRO_FLIP_HORIZONTAL);
        przeciwnik.x = posX;
        przeciwnik.y = posY;
        if ( al_get_time() > czas1 + szybkosc_przeciwnikow)
        {
            posY = posY+1.0;
            czas1 = al_get_time();
        }
    }
};

class klasa_samochod
{
public:
    float posX;
    float posY;
    double czas = al_get_time();
    void init()
    {
        posY = 520;
        posX = 375;
    }

    void update(ALLEGRO_KEYBOARD_STATE keyboard)
    {

        samochod.x = posX;
        samochod.y = posY;
        if ( al_get_time() > czas + 0.005)
        {
            if (al_key_down(&keyboard, ALLEGRO_KEY_UP) && posY > 0)
                posY-=1;
            if (al_key_down(&keyboard, ALLEGRO_KEY_DOWN) && posY < 580 - 55)
                posY+=1;
            if (al_key_down(&keyboard, ALLEGRO_KEY_LEFT) && posX > 0 + 120)
                posX-=1;
            if (al_key_down(&keyboard, ALLEGRO_KEY_RIGHT) && posX < 800 - 170)
                posX+=1;
            czas = al_get_time();
        }
    }
};

void game(ALLEGRO_DISPLAY *mainWindow)
{
    ALLEGRO_FONT *fontPoints, *fontText;                                                                //czcionki
    ALLEGRO_BITMAP *postac, *tlo, *wrog;
    ALLEGRO_SAMPLE *soundNextLevel, *soundGameOver;

    //---wczytywanie potrzebnych zasobów i zakoñczenie funkcji w przypadku niepowodzenia

    fontPoints = al_load_ttf_font("resources/DJB.ttf", 24, 0);
    if(!fontPoints)
        return;
    fontText = al_load_ttf_font("resources/cm.ttf", 32, 0);
    if(!fontText)
        return;
    postac = al_load_bitmap("resources/postac.png");
    if(!postac)
        return;
    tlo = al_load_bitmap("resources/tlo.jpg");
    if(!tlo)
        return;
    wrog = al_load_bitmap("resources/bialy.png");
    if(!wrog)
        return;
    soundNextLevel = al_load_sample("resources/next level.ogg");
    if(!soundNextLevel)
        return;
    soundGameOver = al_load_sample("resources/game over.ogg");
    if(!soundGameOver)
        return;

    //inicjacja zmiennych
    ALLEGRO_KEYBOARD_STATE keyboard;                                                                    //stan klawiatury
    bool endGame = false;                                                                               //informacja, czy nale¿y zakoñczyæ program
    bool gameOver = false;                                                                              //informacja, czy nale¿y zakoñczyæ aktualn¹ rozgrywkê
    bool restart = false;
    int high=0;
    int level=0;
    float x_drogi=0;
    float wiecej_przeciwnikow = 0;
    int wynik=0;
    int klatka;
    double szybkosc_przeciwnikow = 0;
    vector <klasa_przeciwnik > v_przeciwnik;

    high=readhighscore();

    while(!endGame)                                                                                     //powtarzane do momentu zakoñczenia programu
    {
        klasa_przeciwnik obj_przeciwnik;
        klasa_samochod pojazd;
        wynik = 0;
        level = 1;
        klatka = 0;
        szybkosc_przeciwnikow = 0.0025;
        wiecej_przeciwnikow = 0.35;
        double czas2 = al_get_time();
        double czas3 = al_get_time();
        double czas4 = al_get_time();
        double czas5 = al_get_time();
        v_przeciwnik.clear();
        pojazd.init();

        while(!gameOver)                                                                                //powtarzane do czasu zakoñczenie aktualnej rozgrywki (b³êdu gracza_
        {


            high=readhighscore();
            al_get_keyboard_state(&keyboard);

            for (int i=0; i<10; i++)
            {
                x_drogi = i*100 + klatka%100;
                al_draw_bitmap(tlo, 0, x_drogi-200, 0);
            }

            al_draw_bitmap(postac, samochod.x, samochod.y, ALLEGRO_FLIP_HORIZONTAL);
            drawScoreBoard(wynik, high, level, fontText, fontPoints);

            pojazd.update(keyboard);

            if ( al_get_time() > czas4 + wiecej_przeciwnikow)
            {
                czas4 = al_get_time();
                obj_przeciwnik.init();
                v_przeciwnik.push_back(obj_przeciwnik);
            }

            for (unsigned int j=0; j<v_przeciwnik.size(); j++)
            {
                v_przeciwnik.at(j).update(wrog, szybkosc_przeciwnikow);
                if (samochod.x+43>przeciwnik.x && samochod.x<przeciwnik.x+43 && samochod.y+48>przeciwnik.y && samochod.y<przeciwnik.y+48)
                {
                    savehighscore(wynik);
                    gameOver = true;
                    al_play_sample(soundGameOver, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
                }
            }

            if ( al_get_time() > czas5 + 0.004)
            {
                klatka++;
                czas5 = al_get_time();
            }

            if ( al_get_time() > czas2 + 1.0)
            {
                wynik++;
                czas2 = al_get_time();
            }

            if ( wynik%20 == 0 && al_get_time() > czas3 + 2.0)
            {
                wiecej_przeciwnikow -= wiecej_przeciwnikow*0.05;
                if (szybkosc_przeciwnikow > 0.0017)
                    szybkosc_przeciwnikow -= szybkosc_przeciwnikow*0.025;
                level++;
                al_play_sample(soundNextLevel, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);          //dźwięku awansu
                czas3 = al_get_time();
            }


            if(al_key_down(&keyboard, ALLEGRO_KEY_ESCAPE))                                              //jeœli wciœniêto esc
            {
                int decision = showPauseMenu(mainWindow, fontText, fontPoints, wynik);           //wyœwietlenie menu pauzy i pobranie decyzji gracza
                if(decision == 1)                                                                       //jeœli wybrano restart
                {
                    v_przeciwnik.clear();
                    gameOver = true;
                    restart = true;
                }
                else if(decision == 2)                                                                  //jeœli wybrano zakoñczenie gry
                {
                    gameOver = true;
                    endGame = true;
                }
            }

            al_flip_display();  //odœwie¿enie ekranu
        }

        if(!endGame && !restart)                                                                        //jeœli gracz nie wybra³ opcji zakoñczenia gry, ani restartu
            if(showNewGameMenu(mainWindow, fontText, fontPoints, wynik) == 1)                    //wyœwietlenie menu nowej gry i sprawdzenie czy gracz wybra³ opcjê zakoñczenia gry
                endGame = true;
        gameOver = restart = false;
    }

    //---zwolnienie zasobów:
    al_destroy_sample(soundNextLevel);
    al_destroy_sample(soundGameOver);
    al_destroy_font(fontPoints);
    al_destroy_font(fontText);
    al_destroy_bitmap(postac);
    al_destroy_bitmap(tlo);
    al_destroy_bitmap(wrog);
}

int main (void)
{
    if(!initAllegro())                                                                                    //sprawdzenie czy inicjalizacja siê powiod³a
        return -1;

    ALLEGRO_DISPLAY *mainWindow = al_create_display(800, 580);                                           //stworzenie okna programu
    al_set_window_title(mainWindow, "Samochody");                                                            //ustawienie tytu³u okna

    game(mainWindow);

    al_destroy_display(mainWindow);

    return 0;
}
