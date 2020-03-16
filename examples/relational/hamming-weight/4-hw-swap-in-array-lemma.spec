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
   (forall ((k Int) (itK Nat) (itKPlus2 Nat))
      (=>
         (and
            (= (alength t1) (alength t2))

	         (<= 0 k)
            (< (+ k 1) (alength t1))

            (forall ((posA Int))
               (=>
                  (and
                     (not (= posA k) )
                     (not (= posA (+ k 1)) )
                  )
                  (= (a posA t1) (a posA t2))
               )
            )
            (= (a k t1) (a (+ k 1) t2))
            (= (a k t2) (a (+ k 1) t1))


	         (= (i (l12 itK) t1) k)
            (= (i (l12 itKPlus2) t1) (+ k 2))
      	   (= itKPlus2 (s (s itK)))
         )
         (and
	         (= (hammingWeight (l12 itK) t1) (hammingWeight (l12 itK) t2))
	         // (= (hammingWeight (l12 itKPlus2) t1) (hammingWeight (l12 itKPlus2) t2))
	         // (= (hammingWeight main_end t1) (hammingWeight main_end t2))
         )
      )
   )
)
