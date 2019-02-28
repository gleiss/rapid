// if the array is the same in both runs,
// and if z differs at most by k in both runs,
// then the sum of the array-elements and of z differs at most by k.
(two-traces)
func main()
{
  const Int[] a;
  const Int alength;
  const Int z;
  const Int k;
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
  (=>
    (and
      (forall ((j Int))
        (= (a j t1) (a j t2))
      )
      (forall ((xInt Int)(yInt Int)(zInt Int))
        (= (- (+ xInt zInt) (+ yInt zInt)) (- xInt yInt))
      )
      (= (k t1) (k t2))
      (= (alength t1) (alength t2))
      (< (abs(- (z t1) (z t2) )) (k t2))
    )
    (< (abs(- (x main_end t1) (x main_end t2) )) (k t2))
  )
)
