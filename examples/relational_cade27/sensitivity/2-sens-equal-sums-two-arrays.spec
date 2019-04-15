(two-traces)
func main()
{
  const Int[] a;
  const Int[] b;
  const Int alength;
  const Int blength;
  Int x = 0;
  Int i = 0;

  while(i < alength)
  {
    x = x + a[i];
    i = i + 1;
  }

  i = 0;
  while(i < blength)
  {
    x = x + b[i];
    i = i + 1;
  }
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
    )
    (= (x main_end t1) (x main_end t2))
  )
)
