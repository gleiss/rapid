(set-traces 2)
func main()
{
  const Int[] a;
  const Int[] b;
  const Int alength;
  const Int blength;
  const Int k1;
  const Int k2;
  Int x = 0;
  Int y = 0;
  Int z;
  Int i = 0;

  while(i < alength)
  {
    x = x + a[i];
    i = i + 1;
  }
  x = x + k1;

  i = 0;
  while(i < blength)
  {
    y = y + b[i];
    i = i + 1;
  }
  y = y + k2;

  z = x + y;
}


(conjecture
  (=>
    (and
      (forall ((j Int))
        (and
          (= (a j t1) (a j t2))
          (= (b j t1) (b j t2))
        )
      )
      (= (alength t1) (alength t2))
      (= (blength t1) (blength t2))
      (= (k1 t1) 1)
      (= (k1 t2) 0)
      (= (k2 t1) 0)
      (= (k2 t2) 1)
    )
     (= (z main_end t1) (z main_end t2))
  )
)
