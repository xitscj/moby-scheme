/*
 * ProGuard -- shrinking, optimization, obfuscation, and preverification
 *             of Java bytecode.
 *
 * Copyright (c) 2002-2007 Eric Lafortune (eric@graphics.cornell.edu)
 *
 * This library is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation; either version 2 of the License, or (at your option)
 * any later version.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License
 * for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation,
 * Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */
package proguard.classfile.attribute.annotation;

import proguard.classfile.*;
import proguard.classfile.attribute.annotation.visitor.ElementValueVisitor;
import proguard.classfile.visitor.ClassVisitor;

/**
 * This ElementValue represents an enumeration constant element value.
 *
 * @author Eric Lafortune
 */
public class EnumConstantElementValue extends ElementValue
{
    public int u2typeNameIndex;
    public int u2constantNameIndex;

    /**
     * An extra field pointing to the Clazz objects referenced in the
     * type name string. This field is typically filled out by the <code>{@link
     * proguard.classfile.util.ClassReferenceInitializer
     * ClassReferenceInitializer}</code>.
     * References to primitive types are ignored.
     */
    public Clazz[] referencedClasses;


    /**
     * Creates an uninitialized EnumConstantElementValue.
     */
    public EnumConstantElementValue()
    {
    }


    /**
     * Applies the given visitor to all referenced classes.
     */
    public void referencedClassesAccept(ClassVisitor classVisitor)
    {
        if (referencedClasses != null)
        {
            for (int index = 0; index < referencedClasses.length; index++)
            {
                Clazz referencedClass = referencedClasses[index];
                if (referencedClass != null)
                {
                    referencedClass.accept(classVisitor);
                }
            }
        }
    }


    // Implementations for ElementValue.

    public int getTag()
    {
        return ClassConstants.ELEMENT_VALUE_ENUM_CONSTANT;
    }

    public void accept(Clazz clazz, Annotation annotation, ElementValueVisitor elementValueVisitor)
    {
        elementValueVisitor.visitEnumConstantElementValue(clazz, annotation, this);
    }
}
