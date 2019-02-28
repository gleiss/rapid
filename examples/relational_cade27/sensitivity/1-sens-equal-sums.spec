// simplest version: if the array is the same in both runs, then summing all elements gives the same result in both runs

(two-traces)
func main()
{
  const Int[] a;
  const Int alength;
  Int x = 0;
  Int i = 0;

  while(i < alength)
  {
    x = x + a[i];
    i = i + 1;
  }
}

(assert-not
  (=>
    (and
      (forall ((j Int))
        (= (a j t1) (a j t2))
      )
      (= (alength t1) (alength t2))
    )
    (= (x main_end t1) (x main_end t2))
  )
)
