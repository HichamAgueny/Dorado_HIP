set(CMAKE_CXX_COMPILER "/opt/cray/pe/craype/2.7.23/bin/CC")
set(CMAKE_CXX_COMPILER_ARG1 "")
set(CMAKE_CXX_COMPILER_ID "Clang")
set(CMAKE_CXX_COMPILER_VERSION "16.0.6")
set(CMAKE_CXX_COMPILER_VERSION_INTERNAL "")
set(CMAKE_CXX_COMPILER_WRAPPER "CrayPrgEnv")
set(CMAKE_CXX_STANDARD_COMPUTED_DEFAULT "17")
set(CMAKE_CXX_EXTENSIONS_COMPUTED_DEFAULT "ON")
set(CMAKE_CXX_COMPILE_FEATURES "cxx_std_98;cxx_template_template_parameters;cxx_std_11;cxx_alias_templates;cxx_alignas;cxx_alignof;cxx_attributes;cxx_auto_type;cxx_constexpr;cxx_decltype;cxx_decltype_incomplete_return_types;cxx_default_function_template_args;cxx_defaulted_functions;cxx_defaulted_move_initializers;cxx_delegating_constructors;cxx_deleted_functions;cxx_enum_forward_declarations;cxx_explicit_conversions;cxx_extended_friend_declarations;cxx_extern_templates;cxx_final;cxx_func_identifier;cxx_generalized_initializers;cxx_inheriting_constructors;cxx_inline_namespaces;cxx_lambdas;cxx_local_type_template_args;cxx_long_long_type;cxx_noexcept;cxx_nonstatic_member_init;cxx_nullptr;cxx_override;cxx_range_for;cxx_raw_string_literals;cxx_reference_qualified_functions;cxx_right_angle_brackets;cxx_rvalue_references;cxx_sizeof_member;cxx_static_assert;cxx_strong_enums;cxx_thread_local;cxx_trailing_return_types;cxx_unicode_literals;cxx_uniform_initialization;cxx_unrestricted_unions;cxx_user_literals;cxx_variadic_macros;cxx_variadic_templates;cxx_std_14;cxx_aggregate_default_initializers;cxx_attribute_deprecated;cxx_binary_literals;cxx_contextual_conversions;cxx_decltype_auto;cxx_digit_separators;cxx_generic_lambdas;cxx_lambda_init_captures;cxx_relaxed_constexpr;cxx_return_type_deduction;cxx_variable_templates;cxx_std_17;cxx_std_20;cxx_std_23")
set(CMAKE_CXX98_COMPILE_FEATURES "cxx_std_98;cxx_template_template_parameters")
set(CMAKE_CXX11_COMPILE_FEATURES "cxx_std_11;cxx_alias_templates;cxx_alignas;cxx_alignof;cxx_attributes;cxx_auto_type;cxx_constexpr;cxx_decltype;cxx_decltype_incomplete_return_types;cxx_default_function_template_args;cxx_defaulted_functions;cxx_defaulted_move_initializers;cxx_delegating_constructors;cxx_deleted_functions;cxx_enum_forward_declarations;cxx_explicit_conversions;cxx_extended_friend_declarations;cxx_extern_templates;cxx_final;cxx_func_identifier;cxx_generalized_initializers;cxx_inheriting_constructors;cxx_inline_namespaces;cxx_lambdas;cxx_local_type_template_args;cxx_long_long_type;cxx_noexcept;cxx_nonstatic_member_init;cxx_nullptr;cxx_override;cxx_range_for;cxx_raw_string_literals;cxx_reference_qualified_functions;cxx_right_angle_brackets;cxx_rvalue_references;cxx_sizeof_member;cxx_static_assert;cxx_strong_enums;cxx_thread_local;cxx_trailing_return_types;cxx_unicode_literals;cxx_uniform_initialization;cxx_unrestricted_unions;cxx_user_literals;cxx_variadic_macros;cxx_variadic_templates")
set(CMAKE_CXX14_COMPILE_FEATURES "cxx_std_14;cxx_aggregate_default_initializers;cxx_attribute_deprecated;cxx_binary_literals;cxx_contextual_conversions;cxx_decltype_auto;cxx_digit_separators;cxx_generic_lambdas;cxx_lambda_init_captures;cxx_relaxed_constexpr;cxx_return_type_deduction;cxx_variable_templates")
set(CMAKE_CXX17_COMPILE_FEATURES "cxx_std_17")
set(CMAKE_CXX20_COMPILE_FEATURES "cxx_std_20")
set(CMAKE_CXX23_COMPILE_FEATURES "cxx_std_23")

set(CMAKE_CXX_PLATFORM_ID "Linux")
set(CMAKE_CXX_SIMULATE_ID "")
set(CMAKE_CXX_COMPILER_FRONTEND_VARIANT "GNU")
set(CMAKE_CXX_SIMULATE_VERSION "")




set(CMAKE_AR "/pfs/lustrep2/projappl/project_462000125/samantao-public/rocm/rocm-5.7.1/llvm/bin/llvm-ar")
set(CMAKE_CXX_COMPILER_AR "/pfs/lustrep2/projappl/project_462000125/samantao-public/rocm/rocm-5.7.1/llvm/bin/llvm-ar")
set(CMAKE_RANLIB "/pfs/lustrep2/projappl/project_462000125/samantao-public/rocm/rocm-5.7.1/llvm/bin/llvm-ranlib")
set(CMAKE_CXX_COMPILER_RANLIB "/pfs/lustrep2/projappl/project_462000125/samantao-public/rocm/rocm-5.7.1/llvm/bin/llvm-ranlib")
set(CMAKE_LINKER "/pfs/lustrep2/projappl/project_462000125/samantao-public/rocm/rocm-5.7.1/llvm/bin/ld.lld")
set(CMAKE_MT "")
set(CMAKE_TAPI "CMAKE_TAPI-NOTFOUND")
set(CMAKE_COMPILER_IS_GNUCXX )
set(CMAKE_CXX_COMPILER_LOADED 1)
set(CMAKE_CXX_COMPILER_WORKS TRUE)
set(CMAKE_CXX_ABI_COMPILED TRUE)

set(CMAKE_CXX_COMPILER_ENV_VAR "CXX")

set(CMAKE_CXX_COMPILER_ID_RUN 1)
set(CMAKE_CXX_SOURCE_FILE_EXTENSIONS C;M;c++;cc;cpp;cxx;m;mm;mpp;CPP;ixx;cppm;ccm;cxxm;c++m)
set(CMAKE_CXX_IGNORE_EXTENSIONS inl;h;hpp;HPP;H;o;O;obj;OBJ;def;DEF;rc;RC)

foreach (lang C OBJC OBJCXX)
  if (CMAKE_${lang}_COMPILER_ID_RUN)
    foreach(extension IN LISTS CMAKE_${lang}_SOURCE_FILE_EXTENSIONS)
      list(REMOVE_ITEM CMAKE_CXX_SOURCE_FILE_EXTENSIONS ${extension})
    endforeach()
  endif()
endforeach()

set(CMAKE_CXX_LINKER_PREFERENCE 30)
set(CMAKE_CXX_LINKER_PREFERENCE_PROPAGATES 1)
set(CMAKE_CXX_LINKER_DEPFILE_SUPPORTED TRUE)

# Save compiler ABI information.
set(CMAKE_CXX_SIZEOF_DATA_PTR "8")
set(CMAKE_CXX_COMPILER_ABI "ELF")
set(CMAKE_CXX_BYTE_ORDER "LITTLE_ENDIAN")
set(CMAKE_CXX_LIBRARY_ARCHITECTURE "x86_64-unknown-linux-gnu")

if(CMAKE_CXX_SIZEOF_DATA_PTR)
  set(CMAKE_SIZEOF_VOID_P "${CMAKE_CXX_SIZEOF_DATA_PTR}")
endif()

if(CMAKE_CXX_COMPILER_ABI)
  set(CMAKE_INTERNAL_PLATFORM_ABI "${CMAKE_CXX_COMPILER_ABI}")
endif()

if(CMAKE_CXX_LIBRARY_ARCHITECTURE)
  set(CMAKE_LIBRARY_ARCHITECTURE "x86_64-unknown-linux-gnu")
endif()

set(CMAKE_CXX_CL_SHOWINCLUDES_PREFIX "")
if(CMAKE_CXX_CL_SHOWINCLUDES_PREFIX)
  set(CMAKE_CL_SHOWINCLUDES_PREFIX "${CMAKE_CXX_CL_SHOWINCLUDES_PREFIX}")
endif()





set(CMAKE_CXX_IMPLICIT_INCLUDE_DIRECTORIES "/opt/cray/pe/libsci/23.09.1.1/CRAY/12.0/x86_64/include;/opt/cray/pe/mpich/8.1.27/ofi/cray/14.0/include;/opt/cray/pe/hdf5/1.12.2.7/crayclang/14.0/include;/opt/cray/pe/dsmml/0.2.2/dsmml/include;/opt/cray/xpmem/2.5.2-2.4_3.50__gd0f7936.shasta/include;/appl/lumi/SW/LUMI-23.09/common/EB/buildtools/23.09/include;/opt/cray/pe/cce/16.0.1/cce-clang/x86_64/lib/clang/16/include;/opt/cray/pe/cce/16.0.1/cce/x86_64/include/craylibs;/pfs/lustrep2/projappl/project_462000125/samantao-public/rocm/rocm-5.7.1/include;/pfs/lustrep2/projappl/project_462000125/samantao-public/rocm/rocm-5.7.1/rocthrust/include;/pfs/lustrep2/projappl/project_462000125/samantao-public/rocm/rocm-5.7.1/rocsparse/include;/pfs/lustrep2/projappl/project_462000125/samantao-public/rocm/rocm-5.7.1/rocsolver/include;/pfs/lustrep2/projappl/project_462000125/samantao-public/rocm/rocm-5.7.1/rocrand/include;/pfs/lustrep2/projappl/project_462000125/samantao-public/rocm/rocm-5.7.1/rocprim/include;/pfs/lustrep2/projappl/project_462000125/samantao-public/rocm/rocm-5.7.1/rocfft/include;/pfs/lustrep2/projappl/project_462000125/samantao-public/rocm/rocm-5.7.1/rocblas/include;/pfs/lustrep2/projappl/project_462000125/samantao-public/rocm/rocm-5.7.1/rocalution/include;/pfs/lustrep2/projappl/project_462000125/samantao-public/rocm/rocm-5.7.1/opencl/include;/pfs/lustrep2/projappl/project_462000125/samantao-public/rocm/rocm-5.7.1/oam/include;/pfs/lustrep2/projappl/project_462000125/samantao-public/rocm/rocm-5.7.1/miopen/include;/pfs/lustrep2/projappl/project_462000125/samantao-public/rocm/rocm-5.7.1/hsa/include;/pfs/lustrep2/projappl/project_462000125/samantao-public/rocm/rocm-5.7.1/hipsparse/include;/pfs/lustrep2/projappl/project_462000125/samantao-public/rocm/rocm-5.7.1/hiprand/include;/pfs/lustrep2/projappl/project_462000125/samantao-public/rocm/rocm-5.7.1/hipcub/include;/pfs/lustrep2/projappl/project_462000125/samantao-public/rocm/rocm-5.7.1/hipblas/include;/opt/cray/pe/gcc/10.3.0/snos/include/g++;/opt/cray/pe/gcc/10.3.0/snos/include/g++/x86_64-suse-linux;/opt/cray/pe/gcc/10.3.0/snos/include/g++/backward;/usr/local/include;/usr/include")
set(CMAKE_CXX_IMPLICIT_LINK_LIBRARIES "hdf5_hl;hdf5;hdf5_hl_cpp;hdf5_cpp;xpmem;sci_cray_mpi;sci_cray;dl;mpi_cray;mpi_gtl_hsa;dsmml;stdc++;pgas-shmem;quadmath;modules;fi;craymath;f;u;csup;pthread;atomic;m;stdc++;m;unwind;c;unwind")
set(CMAKE_CXX_IMPLICIT_LINK_DIRECTORIES "/opt/cray/pe/libsci/23.09.1.1/CRAY/12.0/x86_64/lib;/opt/cray/pe/mpich/8.1.27/ofi/cray/14.0/lib;/opt/cray/pe/mpich/8.1.27/gtl/lib;/opt/cray/pe/hdf5/1.12.2.7/crayclang/14.0/lib;/opt/cray/pe/dsmml/0.2.2/dsmml/lib;/opt/cray/pe/cce/16.0.1/cce/x86_64/lib;/opt/cray/xpmem/2.5.2-2.4_3.50__gd0f7936.shasta/lib64;/opt/cray/pe/cce/16.0.1/cce-clang/x86_64/lib/x86_64-unknown-linux-gnu;/opt/cray/pe/gcc/10.3.0/snos/lib/gcc/x86_64-suse-linux/10.3.0;/opt/cray/pe/gcc/10.3.0/snos/lib64;/lib64;/usr/lib64;/lib;/usr/lib;/pfs/lustrep2/projappl/project_462000125/samantao-public/rocm/rocm-5.7.1/hip/lib;/pfs/lustrep2/projappl/project_462000125/samantao-public/rocm/rocm-5.7.1/hsa/lib;/pfs/lustrep2/projappl/project_462000125/samantao-public/rocm/rocm-5.7.1/llvm/lib;/pfs/lustrep2/projappl/project_462000125/samantao-public/rocm/rocm-5.7.1/lib;/pfs/lustrep2/projappl/project_462000125/samantao-public/rocm/rocm-5.7.1/lib64;/pfs/lustrep2/projappl/project_462000125/samantao-public/rocm/rocm-5.7.1/llvm;/appl/lumi/SW/LUMI-23.09/common/EB/buildtools/23.09/lib")
set(CMAKE_CXX_IMPLICIT_LINK_FRAMEWORK_DIRECTORIES "")
