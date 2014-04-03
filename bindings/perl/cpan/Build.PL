use 5.006;
use strict;
use warnings FATAL => 'all';
use Module::Build;

my $class = Module::Build->subclass(
    class => "Module::Build::MorphoDiTa",
    code => <<'SUBCLASS');
sub ACTION_build {
    my $self = shift;
    print STDERR "Building requires C++11 compiler, either g++ 4.7 or newer, or clang++ 3.2 or newer.\n";
    $self->SUPER::ACTION_build;
}
SUBCLASS

my $builder = $class->new(
    module_name          => 'Ufal::MorphoDiTa',
    license              => 'lgpl',
    dist_author          => 'Milan Straka <straka@ufal.mff.cuni.cz>',
    dist_abstract        => 'Bindings to MorphoDiTa library',
    dist_version_from    => 'lib/Ufal/MorphoDiTa.pm',
    release_status       => 'stable',
    configure_requires   => {
        'Module::Build'  => 0,
    },
    build_requires       => {
        'ExtUtils::CBuilder' => 0,
        'Test::More' => 0,
    },
    add_to_cleanup       => [ 'Ufal-MorphoDiTa-*' ],
    extra_compiler_flags => ['-std=c++11', '-fvisibility=hidden'],
    extra_linker_flags   => ['-s', '-lstdc++' ],
    include_dirs         => ['morphodita/include' ],
    c_source             => 'morphodita',
);

$builder->create_build_script();
