// start with an array which contains v at index j. Go through array until v is found at index i.
// Prove that i <= j (j and i don't need to be equal since there could be multiple v in the array).
// similar to https://github.com/sosy-lab/sv-benchmarks/blob/master/c/array-examples/standard_sentinel_true-unreach-call_true-termination.c

func main()
{
	Int[] a;
	const Int j;
	const Int v;
	const Int alength;

	Int i = 0;
	a[j] = v;
	while (i < alength && a[i] != v)
	{
		i = i + 1;
	}
}

(assert-not
	(=>
		(and
			(<= 0 alength)
			(<= 0 j)
			(< j alength)
		)
		(<= (i main_end) j)
	)
)
