// we only have the inequality at the end, so if we would only do forward propagation,
// then we need to propagate equalities over the second loop, which is potentially
// simpler than propagating inequalities.
(set-traces 2)
func main()
{
  const Int[] a;
  const Int[] b;
  const Int alength;
  const Int blength;
  const Int z;
  Int x = 0;

  Int i = 0;
  while(i < alength)
  {
    x = x + a[i];
    i = i + 1;
  }

  i=0;
  while(i < blength)
  {
    x = x + b[i];
    i = i + 1;
  }

  x = x + z;
}

(conjecture
  (forall ((k Int))
    (=>
      (and
        (= (alength t1) (alength t2))
        (= (blength t1) (blength t2))
        (forall ((j Int))
          (and
            (= (a j t1) (a j t2))
            (= (b j t1) (b j t2))
          )
        )
        // (< (abs (- (z t1) (z t2) )) k)
        (< (z t1) (+ (z t2) k) )
        (< (z t2) (+ (z t1) k) )
      )
      //(< (abs(- (x main_end t1) (x main_end t2) )) k)
      (and
        (< (x main_end t1) (+ (x main_end t2) k) )
        (< (x main_end t2) (+ (x main_end t1) k) )
      )
    )
  )
)
