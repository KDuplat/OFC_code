int intcmp(const void *a, const void *b)
{   /* this is only correct if it doesn't overflow */
    return *(const int *)a - *(const int *)b;
}
 
int main(){ int i;
  int nums[5] = {2, 3, 5, 6, 8};
  double numd[5] = {1.,2.0001,3.999, 4.99999999, 5.00000001};
  double u,ua;
  int *ptr ;

  for(i=1; i<=10; i++){

    ptr = bsearch(&i, nums, 5, sizeof(int), intcmp);
  
    if (ptr == NULL){ printf("not found\n");             }
    else            { printf("index = %d\n", (int)(ptr - nums) );}
  }
  
  printf("%d \n\n\n", INT_MAX);

  for(i=1; i<=10; i++){ printf("%d  %d  \n", i,  ibsearch ('i',&i,nums,5)  );}

  for(u=1; u<=10; u+=1.){ printf("%d  %d  \n", (int)floor(u),  ibsearch ('d',&u,numd,5)  ); }
  
  ua=-5.678;
  for(u=-1.678; -10<u; u-=1.){
    printf("%g %g  %d \n", ua, u, doublecompare(&ua,&u) );
  }
  return(0);
}

int main (int argc , char *argv[]) { 
  int tabo[10] = {1, -2, 6, 5, -2, -1, 9, 5, -1, 9};
  int no,n,*res;

  no=10;  icoldupl(tabo,no,&res,&n); icolpr(res,n);
  
  printf("discard -1\n");  icoldiscard(-1,&res,&n); icolpr(res,n);
  
  printf("icoluniq\n");  icoluniq(&res,&n);  icolpr(res,n);
 
  exit(0);
}
