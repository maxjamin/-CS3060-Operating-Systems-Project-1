#include <stdio.h>
/* Created by: Ben Smith
 * Class: CS3060 X01 summer 2021 B1
 * Assignment: Project 1
 */

int main(int argc, char *argv[])
{
  printf("Program 1 by Ben Smith\n");

  for(int i= 0; i < argc; i++)
    {
      printf("Argument %d: %s\n", i, argv[i]);
    }
  printf("Number of arguments printed: %d\n", argc);

}
