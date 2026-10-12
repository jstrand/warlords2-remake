
void __cdecl16far FUN_7bab_21c1(int param_1,int param_2,char *param_3)

{
  undefined2 uVar1;
  undefined2 unaff_SS;
  char local_6 [4];
  
  strcpy((char *)CONCAT22(unaff_SS,local_6),(char *)0x41252616);
  if (param_2 == 0) {
    uVar1 = 0x262e;
  }
  else {
    uVar1 = 0x262a;
  }
  sprintf(param_3,s_CARDS__c_03d__s_4125_261a,(int)local_6[*(int *)(param_1 * 2 + 0xc0)],
          *(undefined2 *)(param_1 * 2 + 0xe0),uVar1,0x4125);
  return;
}


