// Variation 3: Example 2 is extended by adding the Hamming weight of an another array b (identical in both traces). 

(set-traces 2)
func main()
{
	const Int alength;
	const Int[] a;	
	const Int blength;
	const Int[] b;
	const Int  x;
	const Int  y;

	Int i = 0;
	Int hammingWeight = 0;

	while (i < alength)
	{
		i = i + 1;
		if (a[i] != 0) {
			hammingWeight = hammingWeight + 1;
		}
		else {
			skip;
		}
	}

	if (x != 0) {
		hammingWeight = hammingWeight + 1;
	}
	else {
		skip;
	}

	if (y != 0) {
		hammingWeight = hammingWeight + 1;
	}
	else {
		skip;
	}

	i = 0;
	while (i < blength)
	{
		i = i + 1;
		if (b[i] != 0) {
			hammingWeight = hammingWeight + 1;
		}
		else {
			skip;
		}
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
			(= (x t1) (y t2))
			(= (y t1) (x t2))
			(= (alength t1) (alength t2))			
			(= (blength t1) (blength t2))						
		)		
		(= (hammingWeight main_end t1) (hammingWeight main_end t2))
	)
)
