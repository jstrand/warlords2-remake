
undefined2 __rom_strcmp(char *param_1,char *param_2)

{
  char *pcVar1;
  char *pcVar2;
  
  do {
    pcVar2 = param_2;
    pcVar1 = param_1;
    if (*param_1 == '\0') {
      return 1;
    }
    param_1 = (char *)CONCAT22(param_1._2_2_,(char *)param_1 + 1);
    param_2 = (char *)CONCAT22(param_2._2_2_,(char *)param_2 + 1);
  } while (*pcVar1 == *pcVar2);
  return 0;
}


