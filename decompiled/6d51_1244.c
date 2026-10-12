
char * __cdecl16far
FUN_6d51_1244(int param_1,char param_2,undefined2 param_3,undefined2 param_4,char *param_5)

{
  char cVar1;
  uint uVar2;
  char *pcVar3;
  
  if (*(char *)(param_1 * 0x2b + 0x13e3) < '\x02') {
    uVar2 = (uint)*(char *)(param_1 * 0x2b + 0x13e3);
    *(char *)(param_1 * 0x2b + 0x13e3) = *(char *)(param_1 * 0x2b + 0x13e3) + '\x01';
  }
  else {
    uVar2 = (uint)(*(char *)(param_1 * 0x2b + 0x13e4) <= *(char *)(param_1 * 0x2b + 0x13e5));
    cVar1 = *(char *)(param_1 * 0x2b + uVar2 + 0x13e4);
    if (cVar1 <= param_2) {
      return (char *)CONCAT11(0x3c,cVar1);
    }
  }
  *(char *)(param_1 * 0x2b + uVar2 + 0x13e4) = param_2;
  *(undefined2 *)(param_1 * 0x2b + uVar2 * 2 + 0x13e6) = param_3;
  *(undefined2 *)(param_1 * 0x2b + uVar2 * 2 + 0x13ea) = param_4;
  pcVar3 = strcpy((char *)CONCAT22(0x3c04,(char *)(param_1 * 0x2b + uVar2 * 0x10 + 0x13ee)),param_5)
  ;
  return (char *)pcVar3;
}


