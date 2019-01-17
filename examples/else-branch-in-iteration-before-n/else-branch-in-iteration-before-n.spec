// Original Motivation: how to prove that the if-branch is taken exactly in the last iteration of the loop?
// Other variations: there is some property P which eventually gets satisfied. There is a branch which is entered iff P holds. Entering the branch terminates the loop in the next iteration.

// This example is interesting, since 
// - it shows that we need very good support for the theory of Natural numbers with zero,s,p,<
// - Spectre reasons about it like a human
// - Spectre reasons about it completely different from every other tool
// - the loop invariant is quite hard and involves ghost state.
// - lemma 2 highlights something which was missing for the array examples of quit.

// Spectre Encoding
func main()
{
	Int x;

	Int found = 0;
	while(found == 0)
	{
		if(x > 0)
		{
			x = x - 1;
			found = 0;
		}
		else
		{
			found = 1;
		}
	}
}
(assert-not
	(<= (x main_end) 0)
)

// How to prove it? Need two changes:
//
// Change 1: add the-first-change trace-lemma:
//	y(n)=1 and y(zero)=0 => exists 0<=it<n : 
//		and
//	1)		forall it1 < s(it): y(it1)=0
//	2)		forall it1 < it: y(s(it1))=0
//	3)		y(s(it))=1
//
// Change 2: Make semantics less incomplete by adding "Any iteration which has defining properties of n is already n".
//	forall it:
//		=>
//			and
//				forall it1 < it: Cond(it)
//				not Cond(it)
//			it=n	
//
// Then we want proof:
// y changes between zero and n, so that first-change-iteration exists.
// Furthermore the successor of the first-change-iteration satisfies the conditions of n and therefore is n.
// Finally since y is 0 in the first-change iteration and 1 afterwards, the condition does not not hold (sic!) in the first-change iteration.
// Therefore x<=0 and therefore x<=0 in the next iteration.


//	Other tools would read in the following encoding and maybe use the following invariant:
//	func main()
//	{
//		Int x;
//	
//		Bool found = false;
//		while(found == false)
//		{
//			if(x > 0)
//			{
//				x = x - 1;
//			}
//			else
//			{
//				found = true;
//			}
//		}
//	}
//	
//	What's the invariant: Need to prove that 
	((found == 0) || (x<=0 && (found == 1)))

//	Other ideas:
//
//	We can see change 2 as instance of a more general principle: In the natural numbers (and in all other total orders with least elements), for any property P, there is at most one minimal element with that property.
//	So if two elements are the least element, then they are the same.
//	The question now is whether we always should add such lemma whenever we assert the existence of a minimal element.
//	P(n) and not P(n') for all n'<n
//	P(m) and not P(m') for all m'<m
//	=> m'=n'
//
//	The example is hard, because we can't easily conclude that we take the else branch in the previous-to-last-iteration.
//	The reason for that is that we need to conclude that found was never set to true before reaching the else branch.
//	We could so easily if we had found=false in the other branch, which would be semantically the same, but much easier for reasoning.
//	Then the prove would be: found is true at n, so n!=zero since found is false. So n gets its value from p(n). If x>0 at p(n), we would have found(n)=true, therefore not x>0 at p(n). By the semantics of the else-branch we then conclude not x>0.
//	
//	Interestingly, breaks works somewhat like that. So we can probably mimic breaks by using a found-flag, which we set in each branch, including those branches where we set it redundantly to false.