#!/usr/bin/python3
import unittest
import sys; sys.path.append('../'); sys.path.append('./')

from SCons.Script import *
from site_tools.libdep_resolve import *

class LibdepTest(unittest.TestCase):

    def test_from_attributes(self):

        only_name = "bob"
        empty_attr = ("jon", )
        with_attr = ("jim", "hand")

        self.assertEqual (getModuleName(only_name), "bob")
        self.assertEqual (getAttributes(only_name), tuple())
        self.assertEqual (getModuleName(empty_attr), "jon")
        self.assertEqual (getAttributes(empty_attr), tuple())
        self.assertEqual (getModuleName(with_attr), "jim")
        self.assertEqual (len(getAttributes(with_attr)), 1)
        self.assertIn ("hand", getAttributes(with_attr))

        #TODO: makeAttributes

        self.assertEqual (addAttribute("Bob", "attr"), ("Bob", "attr"))
        self.assertEqual (addAttribute(("Bob", "attr"), "attr2"), ("Bob", "attr", "attr2"))

        self.assertEqual (getUnknownAttributes(("link_always", "always_link", "bob")), ["link_always", "bob"])


    def test_find_first_occurrence(self):
        haystack = ["bob", "fred", "jon", "bob"]

        self.assertEqual (findFirstOccurrenceOfElement(haystack, "bob"), 0)
        self.assertEqual (findFirstOccurrenceOfElement(haystack, "fred"), 1)
        self.assertEqual (findFirstOccurrenceOfElement(haystack, "jonbert"), None)

        self.assertEqual (findLastOccurrenceOfElement(haystack, "bob"), 3)
        self.assertEqual (findLastOccurrenceOfElement(haystack, "fred"), 1)
        self.assertEqual (findLastOccurrenceOfElement(haystack, "jonbert"), None)

        self.assertEqual (findFirstOccurrenceOfElements(haystack, ["fred", "jon"]), [1, 2])
        self.assertEqual (findFirstOccurrenceOfElements(haystack, ["fred", "bob"]), [1, 0])
        self.assertEqual (findFirstOccurrenceOfElements(haystack, ["fred", "jonbert"]), [1, None])
        self.assertEqual (findFirstOccurrenceOfElements(haystack, []), [])

        self.assertEqual (getNonexistentElements(["fred", "jonbert"], [1, None]), ["jonbert"])

    ## ---------- Objectify with flags

    # TODO

    ## ---------- apply flag overrides

    # TODO

    def test_duplicate_detection(self):
        # test duplicate detection:
        env = Environment()
        did_fail_successfully = False
        registerLibrary(env, 'double_lib', [])
        try:
            registerLibrary(env, 'double_lib', [])
        except Exception as e:
            # this is expected and correct.
            if "Library double_lib was already registered" in str(e):
                did_fail_successfully = True
                pass

        self.assertTrue (did_fail_successfully, msg= "Double registered library was not detected")


    def test_resolve_easy_dependency(self):
        env = Environment()

        registerStaticDependency(env, "dep1", link_libs=["dep2", "dep3", "dep4"])

        self.assertEqual (getDependenciesOfModule(env, "bernd"), [])
        self.assertEqual (getDependenciesOfModule(env, "dep1"), ["dep2", "dep3", "dep4"])

        self.assertEqual (getOverriddenDependenciesOfModule(env, "bernd", None), [])
        self.assertEqual (getOverriddenDependenciesOfModule(env, "bernd", {}), [])
        self.assertEqual (getOverriddenDependenciesOfModule(env, "dep1", None), ["dep2", "dep3", "dep4"])
        self.assertEqual (getOverriddenDependenciesOfModule(env, "dep1", {}), ["dep2", "dep3", "dep4"])

        self.assertEqual (getOverriddenDependenciesOfModule(env, "dep1", {"dep3": None}), ["dep2", "dep4"])
        self.assertEqual (getOverriddenDependenciesOfModule(env, "dep1", {"dep3": "Joachim"}), ["dep2", ("Joachim", 'overridden'), "dep4"])


    def test_resolve_direct_circular_dependency(self):

        def getModuleDependencyNames(env, name):
            return [getModuleName(dep) for dep in getDependenciesOfModule(env, name)]

        env = Environment()

        #     dep1
        #   v     v
        #  dep2  dep3
        #  v A   v
        #  dep4 </
        #   v
        #  dep5
        optimal = "optimal linking: (dep1, 'header_only') {dep3 dep2} dep4 {dep5 dep2}"
        optimal_len = 6
        # (Elements of Groups marked with "{}" can be in any order)

        registerStaticDependency(env, "dep1", link_libs=["dep2", "dep3"])
        registerStaticDependency(env, "dep2", link_libs=["dep4"])
        registerStaticDependency(env, "dep3", link_libs=["dep4"])
        registerStaticDependency(env, "dep4", link_libs=["dep2", "dep5"])

        assert ('dependencies' in env)
        assert ('dep2' in getModuleDependencyNames(env, 'dep1'))
        assert ('dep4' in getModuleDependencyNames(env, 'dep2'))
        assert ('dep4' in getModuleDependencyNames(env, 'dep3'))



        depmap = generateDependencyMap(env, "dep1") #, explain= True)
        # print ("After generation of dependency map:")
        # print (depmap)
        # print ()

        link_list = resolveDependencies(env, depmap, root_modules='dep1') #, explain=True)
        # print ("\nFinal linking list:\n")
        # print (link_list)
        # print (optimal)
        self.assertTrue (verifyDependencyOrder(env, link_list)) #, explain= True))
        self.assertEqual (link_list[0], ("dep1", 'header_only')) # this is not a really hard requirement (especially not caring about position)
        self.assertEqual (len(link_list), optimal_len)

    def test_resolve_indirect_transitive_circular_dependency(self):

        print (2 * '\n')
        env = Environment()

        #     dep1
        #   v     v
        #  dep2  dep3
        #  v     v
        #  dep4 </
        #   v    |
        #  dep5  |
        #   v    |
        #  dep6 -A
        optimal = "optimal linking: (dep1, 'header_only') {dep2 dep3} dep4 dep5 dep6 dep4"
        optimal_len = 7
        # (Elements of Groups marked with "{}" can be in any order)

        registerStaticDependency(env, "dep1", link_libs=["dep2", "dep3"])
        registerStaticDependency(env, "dep2", link_libs=["dep4"])
        registerStaticDependency(env, "dep3", link_libs=["dep4"])
        registerStaticDependency(env, "dep4", link_libs=["dep5"])
        registerStaticDependency(env, "dep5", link_libs=["dep6"])
        registerStaticDependency(env, "dep6", link_libs=["dep4"])

        # also test, whether it works with attribute
        depmap = generateDependencyMap(env, ("dep1", 'external')) #, explain= True)
        print ("After generation of dependency map:")
        print (depmap)
        print ()

        link_list = resolveDependencies(env, depmap, root_modules="dep1") #, explain= True)
        print ("\nFinal linking list:\n")
        print (link_list)
        print (optimal)
        self.assertTrue (verifyDependencyOrder(env, link_list)) #, explain= True))
        self.assertEqual (link_list[0], ("dep1", 'header_only')) # this is not a really hard requirement (especially not caring about position)
        self.assertEqual (len(link_list), optimal_len)


        printAllRegisteredDependencies(env, root_dependencies=["dep1"])
        print("\n")


    def test_insert_dependencies_into_env(self):
        # TODO

        # 1. for one-module-to-build (with "root")

        # 2. The legacy use where dependencies are written manually
        yield


if __name__ == '__main__':
    unittest.main()