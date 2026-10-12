
char * __cdecl16far FUN_2012_1199(int param_1,int param_2,int param_3,int param_4)

{
  double dVar1;
  double dVar2;
  char *pcVar3;
  uint uVar4;
  
  uVar4 = param_1 - param_3 >> 0xf;
  dVar1 = (double)(int)((param_1 - param_3 ^ uVar4) - uVar4);
  uVar4 = param_2 - param_4 >> 0xf;
  dVar2 = (double)(int)((param_2 - param_4 ^ uVar4) - uVar4);
  dVar1 = dVar2 * dVar2 + dVar1 * dVar1;
  if (0.0 < dVar1 || (dVar1 == 0.0) != 0) {
    sqrt(SUB82(dVar1,0),
         (double)CONCAT62(SUB86(dVar1,0),
                          (uint)(0.0 < dVar1) << 8 | (uint)NAN(dVar1) << 10 |
                          (uint)(dVar1 == 0.0) << 0xe));
    pcVar3 = (char *)__ftol();
  }
  else {
    pcVar3 = (char *)s__x__Use_extended_memory_4125_2703 + 0xd;
  }
  return pcVar3;
}


