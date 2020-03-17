// The program represents RSA exponentiation. The variables i and blenght are of low confidentiality.
// The property expresses non-interference: if variables of low confidentiality have the same value initially, they should have the same value at the end of the program
// In this case, the property should hold
(set-traces 2)
func main()
{
	const Int blength;
	const Int n;
	const Int[] b;
	const Int a;
	Int c = 0;
	Int d = 1;
	Int i = blength;

	while (i >= 0)
	{
		i = i - 1;
		c = 2 * c;
		d = (d * d) mod n;
		if (b[i] == 1) {
			c = c + 1;
			d = (d * a) mod n;
		}
		else {
			skip;
		}
	}
}

(conjecture
	(=>
		(and
			(= (i l11 t1) (i l11 t2))
			(= (blength t1) (blength t2))
		)
		(and
			(= (i main_end t1) (i main_end t2))
			(= (blength t1) (blength t2))
		)
	)
)
