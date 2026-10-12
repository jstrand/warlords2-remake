
char * __cdecl16far strupr(char *s)

{
  byte bVar1;
  byte *pbVar2;
  char *pcVar3;
  uint uVar4;
  undefined2 in_DX;
  byte *pbVar5;
  byte *pbVar6;
  undefined2 uVar7;
  
  uVar7 = (undefined2)((ulong)s >> 0x10);
  pbVar5 = (byte *)s;
  uVar4 = 0xffff;
  pbVar6 = pbVar5;
  do {
    if (uVar4 == 0) break;
    uVar4 = uVar4 - 1;
    pbVar2 = pbVar6;
    pbVar6 = pbVar6 + 1;
  } while (*pbVar2 != 0);
  uVar4 = ~uVar4;
  pbVar6 = pbVar5;
  do {
    pbVar2 = pbVar5;
    pbVar5 = pbVar5 + 1;
    bVar1 = *pbVar2;
    pcVar3 = (char *)(uint)bVar1;
    if (('`' < (char)bVar1) && ((char)bVar1 < '{')) {
      pcVar3 = (char *)(uint)(byte)(bVar1 - 0x20);
    }
    pbVar2 = pbVar6;
    pbVar6 = pbVar6 + 1;
    *pbVar2 = (byte)pcVar3;
    uVar4 = uVar4 - 1;
  } while (uVar4 != 0);
  return (char *)CONCAT22(in_DX,pcVar3);
}


