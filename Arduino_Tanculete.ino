#include "LedControl.h"
#include <LiquidCrystal.h>
LedControl lc = LedControl(12, 11, 10, 1);//DIN, CLK, LOAD, No. DRIVER
LiquidCrystal lcd(2,3,4,5,6,7);

#define JOY1_BUTON_PIN 13
#define JOY1_X_PIN A0
#define JOY1_Y_PIN A1
#define JOY2_BUTON_PIN 8
#define JOY2_X_PIN A3
#define JOY2_Y_PIN A2
#define LCD_CONTRAST_PIN 9

unsigned long ultimulMillis = 0;
int butonJoystick1, butonJoystick2;
int pozitieJoystick1X, pozitieJoystick1Y, pozitieJoystick2X, pozitieJoystick2Y;
int proiectil1X, proiectil1Y, proiectil2X, proiectil2Y;
int viataJucator1, viataJucator2, castigator, pozitieTanc1, pozitieTanc2;
int inceputMesaj, finalMesaj, pozitieMesaj;
int modelMatrice, contorModelMatrice, contorMatriceFinala;
int stareJoc;

enum starileJocului {
  STARE_INITIALA,
  JOCUL_INCEPE_RULAREA,
  JOCUL_RULEAZA,
  STARE_FINALA
};

int matriceLed[8][8] = {
  {0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0}
};

void setup() {
  
  pinMode(JOY1_BUTON_PIN, INPUT);
  digitalWrite(JOY1_BUTON_PIN, HIGH);
  pinMode(JOY2_BUTON_PIN, INPUT);
  digitalWrite(JOY2_BUTON_PIN, HIGH); //JOY_BUTON_PIN are valoarea 1 neapasat si 0 apasat
  
  lc.shutdown(0, false);
  lc.setIntensity(0, 3);
  lc.clearDisplay(0);
  lcd.begin(16,2);
  lcd.clear();

  analogWrite(LCD_CONTRAST_PIN,100);

  stareJoc = STARE_INITIALA;
  viataJucator1 = 2;
  viataJucator2 = 2;
  proiectil1X = 0;
  proiectil2X = 7;
  pozitieTanc1 = 4;
  pozitieTanc2 = 3;
  contorModelMatrice = 0;
  inceputMesaj = 0;
  finalMesaj = 0;
  pozitieMesaj = 16;
  modelMatrice = 0;
  contorMatriceFinala = 0;
}

void afisareMatrice()
{
  for (int i = 0; i < 8; i++)
    for (int j = 0; j < 8; j++)
       lc.setLed(0, i, j, matriceLed[i][j]); 
}

void modelMatriceJ1()
{
  for (int i = 0; i < 4; i++) {
                  matriceLed[i][i] = 1;
                  matriceLed[i][7 - i] = 1;
  }
}

void resetJumatate1Matrice()
{
  for (int i = 0; i < 4; i++) {
                  matriceLed[i][i] = 0;
                  matriceLed[i][7 - i] = 0;
  }
}

void modelMatriceJ2()
{
  for (int i = 4; i < 8; i++) {
                  matriceLed[i][i] = 1;
                  matriceLed[i][7 - i] = 1;
  }
}

void resetJumatate2Matrice()
{
  for(int i = 4; i < 8; i++) {
   matriceLed[i][i] = 0;
   matriceLed[i][7 - i] = 0;
  }
}

void afisareMesajLCD(String mesaj,String mesaj2) //primul mesaj va fi afisat cu autoscroll pe prima linie, iar al 2-lea mesaj in mod normal pe a 2-a linie
{
  lcd.clear();
  lcd.setCursor(pozitieMesaj,0);
  lcd.print(mesaj.substring(inceputMesaj,finalMesaj));//mesajul se afiseaza pe toata dimensiunea ecranului folosind substring()
  if (inceputMesaj == 0 && pozitieMesaj > 0)
      {
        pozitieMesaj--;
        finalMesaj++;
      }
    else if (inceputMesaj == finalMesaj)
               {
                 inceputMesaj = finalMesaj = 0;
                  pozitieMesaj = 16;
               }
            else if (finalMesaj == mesaj.length() && pozitieMesaj == 0) 
                      {
                        inceputMesaj++;
                      } 
                   else {
                          inceputMesaj++;
                          finalMesaj++;
                        }
   lcd.setCursor(0,1);
   lcd.print(mesaj2);
}


int verificareButoaneJoystick()
{
  butonJoystick1 = digitalRead(JOY1_BUTON_PIN);
  butonJoystick2 = digitalRead(JOY2_BUTON_PIN);
  
  if (butonJoystick1 == LOW && butonJoystick2 == LOW) return 3;
  if (butonJoystick1 == LOW) return 1;
  if (butonJoystick2 == LOW) return 2;
  
  return 0;
}

void verificareStareInitiala()
{
  if (verificareButoaneJoystick() == 1 || verificareButoaneJoystick() == 3) modelMatriceJ1();
         else resetJumatate1Matrice();
         
  if (verificareButoaneJoystick() == 2 || verificareButoaneJoystick() == 3) modelMatriceJ2();
         else resetJumatate2Matrice();

  if (verificareButoaneJoystick() == 3) stareJoc = JOCUL_INCEPE_RULAREA; //jocul va incepe daca ambele butoane sunt apasate simultan
  
}

void schimbareMatriceFinala()//functie care afiseaza castigatorul pe matrice
{
  contorMatriceFinala++;
  if (contorMatriceFinala > 5)
        {
          resetMatrice();
          if (castigator == 2)
                {
                  for (int i = 0; i < 8; i++)
                      {
                         matriceLed[i][3] = 1;
                         matriceLed[i][4] = 1;
                      }
                  matriceLed[6][2] = 1;
                  matriceLed[6][5] = 1;
                  matriceLed[5][1] = 1;
                  matriceLed[5][2] = 1;
                  matriceLed[5][5] = 1;
                  matriceLed[5][6] = 1;
                  matriceLed[4][0] = 1;
                  matriceLed[4][1] = 1;
                  matriceLed[4][6] = 1;
                  matriceLed[4][7] = 1;
                  matriceLed[3][0] = 1;
                  matriceLed[3][7] = 1;
                  contorMatriceFinala++;
               }
           else 
              {
               for (int i = 0; i < 8; i++)
                      {
                         matriceLed[i][3] = 1;
                         matriceLed[i][4] = 1;
                     }
                matriceLed[1][2] = 1;
                matriceLed[1][5] = 1;
                matriceLed[2][1] = 1;
                matriceLed[2][2] = 1;
                matriceLed[2][5] = 1;
                matriceLed[2][6] = 1;
                matriceLed[3][0] = 1;
                matriceLed[3][1] = 1;
                matriceLed[3][6] = 1;
                matriceLed[3][7] = 1;
                matriceLed[4][0] = 1;
                matriceLed[4][7] = 1;
                contorMatriceFinala++;                    
              }
       if (verificareButoaneJoystick() == 3) restartJoc();
      }
}

void resetMatrice()
{
  for (int i = 0; i < 8; i++)
      for (int j = 0; j < 8; j++) matriceLed[i][j] = 0;
}

void schimbareModelMatrice()//pe matrice va fi afisata o numaratoare inversa pana la inceperea jocului
{
  switch (modelMatrice)
  {
    case 0:
            resetMatrice();
            contorModelMatrice++;
            if (contorModelMatrice < 2) modelMatrice = 1;
            if (contorModelMatrice == 2) modelMatrice = 2;
            if (contorModelMatrice == 3) modelMatrice = 3;
            if (contorModelMatrice == 4) modelMatrice = 4;
            if (contorModelMatrice == 5)
                          {  
                            stareJoc = JOCUL_RULEAZA;
                            lcd.clear(); 
                            matriceLed[0][3] = 1;
                            matriceLed[0][4] = 1;
                            matriceLed[1][3] = 1;
                            matriceLed[1][4] = 1;
                            matriceLed[6][3] = 1;
                            matriceLed[6][4] = 1;
                            matriceLed[7][3] = 1;
                            matriceLed[7][4] = 1;
                         }
            break;
    case 1:
            resetMatrice();
            for (int i = 0; i < 8; i++)
                    {
                      matriceLed[i][i] = 1;
                      matriceLed[i][7 - i] = 1;
                    }
            modelMatrice = 0;
            
            break;
    case 2:
            resetMatrice();
            for (int i = 0; i < 8; i++)
                        {
                          matriceLed[1][i] = 1;
                          matriceLed[2][i] = 1;
                          if (i == 3 || i == 4 || i == 5)
                                {
                                  matriceLed[i][0] = 1;
                                  matriceLed[i][1] = 1;
                                  matriceLed[i][3] = 1;
                                  matriceLed[i][4] = 1;
                                  matriceLed[i][6] = 1;
                                  matriceLed[i][7] = 1;
                                }
                        }
            modelMatrice = 0;
            break;
    case 3:
            resetMatrice();
            matriceLed[1][1] = 1;
            matriceLed[1][2] = 1;
            matriceLed[1][6] = 1;
            matriceLed[1][7] = 1;
            matriceLed[2][1] = 1;
            matriceLed[2][2] = 1;
            matriceLed[2][6] = 1;
            matriceLed[2][7] = 1;
            matriceLed[2][0] = 1;
            matriceLed[2][3] = 1;
            matriceLed[3][0] = 1;
            matriceLed[3][3] = 1;
            matriceLed[3][4] = 1;
            matriceLed[3][7] = 1;
            matriceLed[4][0] = 1;
            matriceLed[4][7] = 1;
            matriceLed[4][5] = 1;
            matriceLed[4][4] = 1;
            matriceLed[5][0] = 1;
            matriceLed[5][1] = 1;
            matriceLed[5][5] = 1;
            matriceLed[5][6] = 1;
            matriceLed[5][7] = 1;
            matriceLed[6][1] = 1;
            matriceLed[6][6] = 1;
            matriceLed[6][7] = 1;  
            modelMatrice = 0;

            break;
    case 4:
            resetMatrice();
            for (int i = 0; i < 8; i++)
                    {
                       matriceLed[3][i] = 1;
                       matriceLed[4][i] = 1;
                    }
            matriceLed[5][1] = 1;
            matriceLed[5][2] = 1;
            matriceLed[5][7] = 1;
            matriceLed[6][7] = 1;
            matriceLed[1][7] = 1;
            matriceLed[2][7] = 1;
            modelMatrice = 0;

            break;
  }
}

void afisareMesajLCDinceput()
{
  lcd.clear();
  lcd.setCursor(1,0);
  lcd.print("Jocul incepe!!");
}

void citireJS()
{
  pozitieJoystick1X = analogRead(JOY1_X_PIN);
  pozitieJoystick1Y = analogRead(JOY1_Y_PIN);
  pozitieJoystick2X = analogRead(JOY2_X_PIN);
  pozitieJoystick2Y = analogRead(JOY2_Y_PIN);

  if (pozitieJoystick1X < 300) miscareStanga(1);
      else if (pozitieJoystick1X > 700) miscareDreapta(1);
      
  if (pozitieJoystick2X < 300) miscareStanga(2);
      else if (pozitieJoystick2X > 700) miscareDreapta(2);
}

void miscareStanga(int nrJS)
{
  if (nrJS == 1 && pozitieTanc1 < 7)
  {
    matriceLed[0][pozitieTanc1 + 1] = 1;
    matriceLed[0][pozitieTanc1] = 1;
    matriceLed[1][pozitieTanc1] = 1;
    matriceLed[0][pozitieTanc1 - 1] = 0;
    matriceLed[1][pozitieTanc1 + 1] = 1;
    matriceLed[1][pozitieTanc1 - 1] = 0;
    pozitieTanc1++;
  }
  if (nrJS == 2 && pozitieTanc2 > 0)
  {
    matriceLed[6][pozitieTanc2 - 1] = 1;
    matriceLed[6][pozitieTanc2] = 1;
    matriceLed[7][pozitieTanc2] = 1;
    matriceLed[6][pozitieTanc2 + 1] = 0;
    matriceLed[7][pozitieTanc2 - 1] = 1;
    matriceLed[7][pozitieTanc2 + 1] = 0;
    pozitieTanc2--;
  }
}

void miscareDreapta(int nrJS)
{
  if (nrJS == 1 && pozitieTanc1 >1)
  {
    matriceLed[0][pozitieTanc1 - 2] = 1;
    matriceLed[0][pozitieTanc1 - 1] = 1;
    matriceLed[1][pozitieTanc1 - 1] = 1;
    matriceLed[0][pozitieTanc1] = 0;
    matriceLed[1][pozitieTanc1 - 2] = 1;
    matriceLed[1][pozitieTanc1] = 0;
    pozitieTanc1--;
  }
  if (nrJS == 2 && pozitieTanc2 < 6)
  {
    matriceLed[6][pozitieTanc2] = 0;
    matriceLed[6][pozitieTanc2 + 2] = 1;
    matriceLed[6][pozitieTanc2 + 1] = 1;
    matriceLed[7][pozitieTanc2 + 1] = 1;
    matriceLed[7][pozitieTanc2] = 0;
    matriceLed[7][pozitieTanc2 + 2] = 1;
    pozitieTanc2++;
  }
}

void afisareMesajLCDInJoc()
{
  lcd.clear();
  lcd.setCursor(0,0);
  if (viataJucator1 == 2) lcd.print("J1: 2 vieti");
    else lcd.print("J1: o viata");
              
  lcd.setCursor(0,1);
  if (viataJucator2 == 2) lcd.print("J2: 2 vieti");
    else lcd.print("J2: o viata");
}

void lansareProiectil()                                                                               //la apasarea butonului se va lansa un proiectil
{                                                                                                     //fiecare tanc poate trage doar cu cate 1 proiectil pe rand
  if ( (verificareButoaneJoystick() == 1 || verificareButoaneJoystick() == 3) && proiectil1X == 0)    //tancul poate trage un proiectil nou doar dupa ce vede daca a nimerit sau nu tinta
  { 
     proiectil1X = 1;
     proiectil1Y = pozitieTanc1;
  } 

   if ( (verificareButoaneJoystick() == 2 || verificareButoaneJoystick() == 3) && proiectil2X == 7) 
   {
     proiectil2X = 6;
     proiectil2Y = pozitieTanc2;
   }
   
   miscareProiectil();
}

void miscareProiectil()
{
  if (proiectil1X > 0 && proiectil1X < 7)
  {
    proiectil1X++;
    matriceLed[proiectil1X][proiectil1Y] = 1;
    if (proiectil1X != 2) matriceLed[proiectil1X - 1][proiectil1Y] = 0;
    if (proiectil1X == 6 && matriceLed[7][proiectil1Y] == 1)
                            {
                               viataJucator2--;
                               proiectil1X = 0;
                               if (viataJucator2 == 0)
                                          { 
                                              stareJoc = STARE_FINALA;        //daca proiectilul a distrus inamicul, atunci se va anunta castigatorul
                                              castigator = 1;
                                          }
                            }
  }
  
  if (proiectil1X == 7)
        {
          matriceLed[proiectil1X][proiectil1Y] = 0;
          proiectil1X = 0;
        }
        
  if (proiectil2X < 7 && proiectil2X > 0)
  {
    proiectil2X--;
    matriceLed[proiectil2X][proiectil2Y]=1;
    if(proiectil2X != 5) matriceLed[proiectil2X + 1][proiectil2Y] = 0;
    if(proiectil2X == 1 && matriceLed[0][proiectil2Y] == 1)
                            {
                                viataJucator1--;
                                proiectil2X = 7;
                                if(viataJucator1 == 0)
                                          { 
                                              stareJoc = STARE_FINALA;
                                              castigator = 2;
                                          }
                            }
  }
  
  if(proiectil2X == 0)
        {
          matriceLed[proiectil2X][proiectil2Y] = 0;
          proiectil2X = 7;
        }
}

void afisareMesajLCDfinal()
{
  lcd.clear();
  afisareMesajLCD("Pentru revansa apasati pe Joystick","");
  lcd.setCursor(0,1);
  if (castigator == 1) lcd.print("Felicitari J1 !!");
    else lcd.print("Felicitari J2 !!");
}

void restartJoc()
{
  stareJoc = JOCUL_INCEPE_RULAREA;
  viataJucator1 = 2;
  viataJucator2 = 2;
  pozitieTanc1 = 4;
  pozitieTanc2 = 3;
  contorModelMatrice = 0;
  modelMatrice = 0;
  contorMatriceFinala = 0;
  proiectil1X = 0;
  proiectil2X = 7;
  proiectil1Y = pozitieTanc1;
  proiectil2Y = pozitieTanc2;
  inceputMesaj = 0;
  finalMesaj = 0;
  pozitieMesaj = 16;
}
void loop() {

  switch(stareJoc)
  {
    case STARE_INITIALA: 
                           if (millis() - ultimulMillis > 500)
                                {           
                                  ultimulMillis = millis();
                                  verificareStareInitiala();
                                  afisareMatrice();
                                  afisareMesajLCD("Apasati pe Joystick pentru a incepe!","Bine ati venit! ");
                                }
                           break;
                  
    case JOCUL_INCEPE_RULAREA: 
                                if (millis() - ultimulMillis > 800)
                                    {          
                                      ultimulMillis = millis();
                                      schimbareModelMatrice();
                                      afisareMatrice();
                                      afisareMesajLCDinceput();     
                                    }
                                 break;    
                 
    case JOCUL_RULEAZA: 
                         if (millis() - ultimulMillis > 125)
                              {          
                                ultimulMillis = millis();
                                lansareProiectil();
                                citireJS();
                                afisareMatrice();
                                afisareMesajLCDInJoc();       
                              }    
                        break;
            
    case STARE_FINALA: 
                        if (millis() - ultimulMillis > 500)
                             { 
                              ultimulMillis = millis();
                              afisareMesajLCDfinal();
                              schimbareMatriceFinala();
                              afisareMatrice();
                             }
                        break;
  }
}
