%module "Ufal::MorphoDiTa"

%runtime %{
#ifdef seed
  #undef seed
#endif
%}

%include "../common/morphodita.i"

%perlcode %{
@EXPORT_OK = qw(*Forms:: *TaggedForm:: *TaggedForms:: *TaggedLemma:: *TaggedLemmas::
                *TaggedLemmaForms:: *TaggedLemmasForms:: *TokenRange:: *TokenRanges::
                *Tokenizer:: *Morpho:: *Tagger:: *TagsetConverter:: *Version::);
%EXPORT_TAGS = (all => [@EXPORT_OK]);

=head1 NAME

UFal::MorphoDiTa - bindings to Morphodita library L<http://ufal.mff.cuni.cz/morphodita>.

=head1 DESCRIPTION

C<Ufal::MorphoDiTa> is a Perl binding to Morphodita library L<http://ufal.mff.cuni.cz/morphodita>.

All classes can be imported into the current namespace using the C<all> export tag.

The bindings is a straightforward conversion of the C<C++> bindings API.
Vectors do not have native Perl interface, see L<Ufal::MorphoDiTa::Forms>
source for reference. Static methods and enumerations are available only
through the module, not through object instance.

=head1 AUTHOR

Milan Straka <straka@ufal.mff.cuni.cz>

Jana Straková <strakova@ufal.mff.cuni.cz>

=head1 COPYRIGHT AND LICENCE

Copyright 2014 by Institute of Formal and Applied Linguistics, Faculty of
Mathematics and Physics, Charles University in Prague, Czech Republic.

MorphoDiTa is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as
published by the Free Software Foundation, either version 3 of
the License, or (at your option) any later version.

MorphoDiTa is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with MorphoDiTa.  If not, see <http://www.gnu.org/licenses/>.

=cut

%}
