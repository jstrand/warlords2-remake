
void __cdecl16far hero_add_experience(undefined4 param_1,int param_2)

{
  byte *pbVar1;
  char cVar2;
  byte bVar3;
  int iVar4;
  undefined2 uVar5;
  
  uVar5 = (undefined2)((ulong)param_1 >> 0x10);
  iVar4 = (int)param_1;
  if (*(char *)(iVar4 + 4) == '\x1c') {
    if ((uint)(*(byte *)(*(char *)(iVar4 + 10) + 0x5e3) >> 2) + param_2 < 0x3c) {
      bVar3 = *(byte *)(*(char *)(iVar4 + 10) + 0x5e3);
      cVar2 = *(char *)(iVar4 + 10);
      pbVar1 = (byte *)(cVar2 + 0x5e3);
      *pbVar1 = *pbVar1 & 3;
      pbVar1 = (byte *)(cVar2 + 0x5e3);
      *pbVar1 = *pbVar1 | ((bVar3 >> 2) + (char)param_2) * '\x04';
    }
    else {
      cVar2 = *(char *)(iVar4 + 10);
      pbVar1 = (byte *)(cVar2 + 0x5e3);
      *pbVar1 = *pbVar1 & 3;
      pbVar1 = (byte *)(cVar2 + 0x5e3);
      *pbVar1 = *pbVar1 | 0xf0;
    }
  }
  return;
}


