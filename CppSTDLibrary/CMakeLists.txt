cmake_minimum_required(VERSION 3.16)
project(CppSTDLibrary)

set(CMAKE_CXX_STANDARD 26)
# add_compile_options(-c -Wall -Wextra -O3 -std=c++26)
add_compile_options(-c -std=c++26 -Wall -O3 -fcontracts)

add_executable(${PROJECT_NAME}
        main.cpp
		Types.cpp
		AggregateInitialization/AggregateInitialization.cpp
		Algorithms/Algorithms.cpp
		Any/Any.cpp
		Arrays/Arrays.cpp
		Asserts/Asserts.cpp
		Atomic/Atomic.cpp
		Attributes/Attributes.cpp
		Attributes/Assume.cpp
		Auto/Auto.cpp
		BitSet/BitSet.cpp
		BitwiseOperation/BitwiseOperation.cpp
		BitwiseOperation/Count_Zeros.cpp
		Byte/Byte.cpp
		Chrono/Chrono.cpp
		Chrono/TimeZones.cpp
		Concepts/Concepts.cpp
		Contracts/Contracts.cpp
		CTAD/ClassTemplateArgumentDeduction.cpp
		ConsoleInOut/ConsoleInOut.cpp
		DateAndTime/DateAndTime.cpp
		DeducingThis/DeducingThis.cpp
		CompileTime_Constexpr/ConstConstexprMutable.cpp
		CompileTime_Constexpr/CompileTimeProgramming.cpp
        CompileTime_Constexpr/Constexpr.cpp
		CompileTime_Constexpr/GetClassName_CompileTime.cpp
		EnumTests/EnumTests.cpp
		Embed/BinaryResourceInclusion.cpp
		Exceptions/Exceptions.cpp
		Errors/Errors.cpp
		Helpers/UnorderedPair.h
		FilesStreams/FilesStreams.cpp
		Filesystem/Filesystem.cpp
		Format/Format.cpp
		Format/CustomTypesFormatters.cpp
        Functional_Objects/FunctionObjects.cpp
        Functional_Objects/MemberFunctionPointer.cpp
        Functional_Objects/CallbackTests.cpp
        Functional_Objects/MoveOnlyFunction.cpp
        Functional_Objects/PassCallableToClass.cpp
        Functional_Objects/SmallFunctionWrapper.cpp
		Initialization/Initialization.cpp
		InitializerList/InitializerList.cpp
		InlineVariables/InlineVariables.cpp
		Helpers/Integer.cpp
		Iterators/Iterators.cpp
		Iterators/Custom_Iterators.cpp
		LookupTypes/LookupTypes.cpp
		Lambdas/Lambdas.cpp
		LifetimeExtension/LifetimeExtension.cpp
		Literals/Literals.cpp
		Locale/Locale.cpp
		Overflow/Overflow.cpp
		Math/Math.cpp
		Memory/Memory.cpp
		Memory/MemoryPool.cpp
		Memory/MemoryUsageMonitor.cpp
		Memory/Alignment.cpp
		Memory/Launder.cpp
		Memory/AlignedStackAllocator.cpp
		Memory/CustomStackAllocator.cpp
		MoveSemantics/MoveSemantics.cpp
		Numbers/Numbers.cpp
		Reflection/Reflection.cpp
		NumericLimits/NumericLimits.cpp
		ObjectOrientedProgramming/ObjectOrientedProgramming.cpp
		ObjectOrientedProgramming/Operators.cpp
		ObjectOrientedProgramming/RefQualifierMemberFunctions.cpp
		ObjectOrientedProgramming/ConstexprVirtualFunctions.cpp
        Optional/Optional.cpp
        Optional/Custom_Optional.cpp
		PolymorphicMemoryResources/PolymorphicMemoryResources.cpp
		Random/Random.cpp

		Ranges_Views/Ranges.cpp
		Ranges_Views/Store.cpp

		ReferenceWrapper/ReferenceWrapper.cpp
		SharedPtr/SharedPtrTests.cpp
		SourceLocation/SourceLocation.cpp
		Span/Span.cpp Span/Span.h
		Print/Print.cpp Print/Print.h
		Simd/Simd.cpp Simd/Simd.h
		Streams/Streams.cpp Streams/Streams.h
		String/String.cpp
		String/StringUtilities.cpp
		StringStream/StringStreamTests.cpp

		StringView/StringViewTests.cpp
		StringView/Number_Parser.cpp

		StructuredBinding/StructuredBinding.cpp
		StrongTypes/StrongTypes.cpp
		Tuple/TupleTests.cpp
		TypeCast/TypeCast.cpp
		TypeTraits/TypeTraits.cpp
		UniquePtr/UniquePtr.cpp

		Variant/Variant.cpp
		Variant/Matcher_Visitor.cpp
		Variant/ParsersVariantDemo.cpp

		WeakPtr/WeakPtr.cpp
		WeakPtr/WeakPtr_MakeShared.cpp
        Volatile/VolatileTests.cpp
		Hashing/Hashing.cpp Hashing/Hashing_Utilities.cpp
		Metaprogramming/Metaprogramming.cpp
		StarshipOperator/StarshipOperator.cpp
		Expected/Expected.cpp
		Types/Int.cpp
		NameSpaces/InlineNamespaces.cpp
		Comparators/Comparators.cpp

		ScopeExit_Experimental/ScopeExit.cpp

		Templates/Templates.cpp
		Templates/ConditionalExplicit.cpp
		Templates/FoldExpressions.cpp
		Templates/NTTP.cpp
		Templates/SFINAE.cpp
		Templates/PackIndexing.cpp
		Templates/TemplateSpecialization.cpp
		Templates/TypeListChecker.cpp
		Templates/IndexSequence.cpp

		RVO_CopyElision/CopyElision.cpp
		RVO_CopyElision/RVO_FailureCases.cpp
		Overflow/Overflow.cpp
		StaticAsserts/StaticAsserts.cpp
		Stacktrace/Stacktrace.cpp
		ContextControl/ContextControl.cpp
		Helpers/Integer.cpp
		Helpers/Integer.h
        CompileTime_Constexpr/Constexpr.cpp
		CompileTime_Constexpr/Constexpr.hpp
		Memory/ObjectPool_Stack_FixedSize.cpp
		Memory/ObjectPool_Stack_FixedSize.h

		Utilities/Invoke.cpp
		Utilities/Utilities.cpp
		Utilities/Bind.cpp
		Utilities/Decay.cpp
		Utilities/CompilerVersion.cpp
		Utilities/NullPtr_T.cpp
		Utilities/ScopeExit.cpp
		Utilities/Class_Member_Pointer.cpp
)

# Link libraries to the project/executable
target_link_libraries(${PROJECT_NAME} PUBLIC
		pthread
		${EXTRA_LIBS}
)