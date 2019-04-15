// Variation 4: The fact that the array elements at index x and y
// are swapped in the two traces is expressed in the conjecture's precondition.
(two-traces)
func main()
{
	const Int alength;
	const Int[] a;

	Int i = 0;
	Int hammingWeight = 0;

	while (i < alength)
	{
		hammingWeight = hammingWeight + a[i];
		i = i + 1;
	}
}


(conjecture
   (forall ((k1 Int)(itK1 Nat)(itK1Plus2 Nat))
      (=>
         (and
            (= (alength t1) (alength t2))
            (<= 0 k1)
            (< (+ k1 1) (alength t1))
            (= (i (l12 itK1) t1) k1)
            (= (i (l12 itK1Plus2) t1) (+ k1 2))
            (= itK1Plus2 (s (s itK1)))
            (forall ((v Int))
               (=>
                  (and
                     (not
                        (= v k1)
                     )
                     (not
                        (= v (+ k1 1))
                     )
                  )
                  (= (a v t1) (a v t2))
               )
            )
            (= (a k1 t1) (a (+ k1 1) t2))
            (= (a k1 t2) (a (+ k1 1) t1))
            // Theory Axiom
            (forall ((x Int)(y Int) (z Int))
              (= (+ (+ x y) z)  (+ (+ x z) y))
            )
            // proven in hamming_weight_k2_end
            (forall ((k Int)(itK Nat)(itKPlus2 Nat))
               (=>
                  (and
                     (= (alength t1) (alength t2))
                     (<= 0 k)
                     (< (+ k 1) (alength t1))
                     (= (i (l12 itK) t1) k)
                     (= (i (l12 itKPlus2) t1) (+ k 2))
                     (= itKPlus2 (s (s itK)))
                     (forall ((v Int))
                        (=>
                           (and
                              (not
                                 (= v k)
                              )
                              (not
                                 (= v (+ k 1))
                              )
                           )
                           (= (a v t1) (a v t2))
                        )
                     )
                     (= (a k t1) (a (+ k 1) t2))
                     (= (a k t2) (a (+ k 1) t1))
                  )
                  (=>
                     (= (hammingWeight (l12 itKPlus2) t1) (hammingWeight (l12 itKPlus2) t2))
                     (= (hammingWeight main_end t1) (hammingWeight main_end t2))
                  )
               )
            )
         )
         (= (hammingWeight main_end t1) (hammingWeight main_end t2))
      )
   )
)
