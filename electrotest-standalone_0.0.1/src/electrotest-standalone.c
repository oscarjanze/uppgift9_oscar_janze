#include <stdio.h>
#include <stdlib.h>
#include <math.h>

float e12[] = {1.0f, 1.2f, 1.5f, 1.8f, 2.2f, 2.7f, 3.3f, 3.9f, 4.7f, 5.6f, 6.8f, 8.2f};

float calc_power_r(float volt, float resistance);
float calc_power_i(float volt, float current);
int e_resistance(float orig_resist, float *res_array);
float calc_resistance(int count, char conn, float *array);
static int find_decade(float res);
static int find_resistance(float *res, int dec);


int main()
{
   char connection;
   int comp, count;
   float voltage, total_resistance;
   float *array;


   printf("Ange spänningskälla i V: ");
   scanf("%f", &voltage);

   //Känslig scanf. Mellanrummet på " %c" ska vara där
   //Gör fallgrop för andra tecken än S(s) och P(p)
   printf("Ange koppling[S|P]: ");
   scanf(" %c", &connection);

   printf("Antal komponenter: ");
   scanf("%d", &comp);

   //Dynamisk allokering av resistansarrayen
   array = (float*)malloc(comp*sizeof(float));

   for (int i = 0; i < comp; i++)
   {
      printf("Komponent %i i ohm: ", i+1);
      scanf("%f", &array[i]);
   }

   //Test för libresistance
   total_resistance = calc_resistance(comp, connection, &array[0]);
   printf("Ersättningsresistans:\n");
   printf("%.1f ohm\n", total_resistance);

   //Allokerat minne för array används ej längre
   free(array);

   //Test för libpower
   printf("Effekt:\n");
   printf("%.2f\n", calc_power_r(voltage, total_resistance));

   //Test för libcomponent
   //Allokera minne för tre ersättningsresistanser
   array = (float*)malloc(3*sizeof(float));
   count = e_resistance(total_resistance, &array[0]);
   printf("Ersättningsresistanser i E12-serien kopplade i serie: \n");
   for (int i = 0; i < count; ++i)
   {
      printf("%.1f\n", array[i]);
   }

   //Allokerat minne för ersättningsresistanser används ej längre
   free(array);

   return 0;
}



int e_resistance(float orig_resist, float *res_array)
{
  int i, decade, index;
  int count = 0;
  float resistance = orig_resist;

  //Assumes resistances available range from 1hm to 10⁸Ohms
  if(orig_resist < pow(10,8) ||
     orig_resist >= 1)
  {
    for(i = 0; i<=3; i++)
    {
      decade = find_decade(resistance);
      index = find_resistance(&resistance, decade);

      res_array[i] = e12[index] * pow(10,decade);
      count++;
      if(resistance < 1 || count >= 3)
      {
        break;
      }
    }
  }
  else
  {
    //Assume too large resistance
    count = 0;
  }

  return count;
}

static int find_resistance(float *res, int decade)
{
  int i;
  float resistance = (*res)/pow(10,decade);

  for(i = 11; i >= 0; i--)
  {
    if(resistance >= e12[i])
    {
      *res = (resistance - e12[i]) * pow(10, decade);
      break;
    }
  }

  return i;
}

static int find_decade(float res)
{
  int i;

  for(i = 1; i < 8; i++)
  {
    if(res < pow(10,i))
    {
      break;
    }
  }
  return i-1;
}


float calc_resistance(int count, char conn, float *array)
{
    float amountOhm = 0, tmpOhm = 0, divOhm = 0;

    if(array != NULL){
        if(conn != 'S' && conn != 'P'){
                    return -1;
            }
        if(conn == 'S'){
            for(int i = 0; i < count; i++){
                amountOhm += array[i];
            }
            return amountOhm;
        }
        else if(conn == 'P'){
            for(int i = 0; i < count; i++){
                if(array[i]!=0)
                    tmpOhm += (1 / array[i]);
                else
                    return -1;
            }
            divOhm = 1 / tmpOhm;
            amountOhm = divOhm;
            return amountOhm;
        }

        divOhm = 1 / tmpOhm;
        amountOhm = divOhm;
        return amountOhm;
    }
    return -1;
}


float calc_power_r(float volt, float resistance)
{
   return volt*volt/resistance;
}


float calc_power_i(float volt, float current)
{
   return volt*current;
}