// Variation 4: The fact that the array elements at index x and y
// are swapped in the two traces is expressed in the conjecture's precondition.
(set-traces 2)
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

(axiom
	(<= 0 (alength t1))
)
(axiom
	(<= 0 (alength t2))
)

//// Theory Axiom
//(axiom
//   (forall ((x Int)(y Int) (z Int))
//      (= (+ (+ x y) z)  (+ (+ x z) y))
//   )
//)

(conjecture
   (forall ((k1 Int)(itK1 Nat)(itK1Plus2 Nat))
      (=>
         (and
            (= (alength t1) (alength t2))

            (<= 0 k1)
            (< (+ k1 1) (alength t1))
            
            // TODO: only assert the equality for positions in range [0, alength)
            (forall ((posA Int))
               (=>
                  (and
                     (not
                        (= posA k1)
                     )
                     (not
                        (= posA (+ k1 1))
                     )
                  )
                  (= (a posA t1) (a posA t2))
               )
            )
            (= (a k1 t1) (a (+ k1 1) t2))
            (= (a k1 t2) (a (+ k1 1) t1))


            (= (i (l12 itK1) t1) k1)
            (= (i (l12 itK1Plus2) t1) (+ k1 2))

 //           (= itK1Plus2 (s (s itK1))) // TODO: shouldn't be necessary

            // proven in hamming_weight_k2_end
//            (forall ((k Int)(itK Nat)(itKPlus2 Nat))
//               (=>
//                  (and
//                     (= (alength t1) (alength t2))
//                     (<= 0 k)
//                     (< (+ k 1) (alength t1))
//                     (= (i (l12 itK) t1) k)
//                     (= (i (l12 itKPlus2) t1) (+ k 2))
//                     (= itKPlus2 (s (s itK)))
//                     (forall ((v Int))
//                        (=>
//                           (and
//                              (not
//                                 (= v k)
//                              )
//                              (not
//                                 (= v (+ k 1))
//                              )
//                           )
//                           (= (a v t1) (a v t2))
//                        )
//                     )
//                     (= (a k t1) (a (+ k 1) t2))
//                     (= (a k t2) (a (+ k 1) t1))
//                  )
//                  (=>
//                     (= (hammingWeight (l12 itKPlus2) t1) (hammingWeight (l12 itKPlus2) t2))
//                     (= (hammingWeight main_end t1) (hammingWeight main_end t2))
//                  )
//               )
//            )
         )
         (= (hammingWeight main_end t1) (hammingWeight main_end t2))
      )
   )
)
