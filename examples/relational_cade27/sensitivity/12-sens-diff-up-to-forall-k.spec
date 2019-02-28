// if the array is the same in both runs,
// and if z differs at most by k in both runs,
// then the sum of the array-elements and of z differs at most by k.

(two-traces)
func main()
{
  const Int[] a;
  const Int alength;
  const Int z;
  Int x = 0;
  Int i = 0;

  while(i < alength)
  {
    x = x + a[i];
    i = i + 1;
  }

  x = x + z;
}

(assert-not
   (forall ((k Int))
      (=>
         (and
            (forall ((j Int))
               (= (a j t1) (a j t2))
            )
            (= (alength t1) (alength t2))
            (= (z t1) (+ (z t2) k))
         )
         (= (x main_end t1) (+ (x main_end t2) k))
      )
   )
)
