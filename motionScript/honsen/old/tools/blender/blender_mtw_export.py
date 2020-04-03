#!BPY

"""
Name: 'Honsen MTW'
Blender: 232
Group: 'Export'
Tooltip: 'Export to MTW format'
"""

import Blender
from Blender import *
from struct import *
from Blender.Mathutils import *
import math, string


# Math stuff

def matrix_rotate(axis, angle):
  vx  = axis[0]
  vy  = axis[1]
  vz  = axis[2]
  vx2 = vx * vx
  vy2 = vy * vy
  vz2 = vz * vz
  cos = math.cos(angle)
  sin = math.sin(angle)
  co1 = 1.0 - cos
  return Matrix(
    [vx2 * co1 + cos,          vx * vy * co1 + vz * sin, vz * vx * co1 - vy * sin, 0.0],
    [vx * vy * co1 - vz * sin, vy2 * co1 + cos,          vy * vz * co1 + vx * sin, 0.0],
    [vz * vx * co1 + vy * sin, vy * vz * co1 - vx * sin, vz2 * co1 + cos,          0.0],
    [0.0, 0.0, 0.0, 1.0],
    )

def blender_bone2matrix(head, tail, roll):
  # Convert bone rest state (defined by bone.head, bone.tail and bone.roll)
  # to a matrix (the more standard notation).
  # Taken from blenkernel/intern/armature.c in Blender source.
  # See also DNA_armature_types.h:47.

  target = Vector([0.0, 1.0, 0.0])
  nor  = Vector([tail[0] - head[0], tail[1] - head[1], tail[2] - head[2]])
  nor.normalize()
  axis = CrossVecs(target, nor)

  if DotVecs(axis, axis) > 0.0000000000001:
    axis.normalize()
    theta = math.acos(DotVecs(target, nor))
    #bMatrix = RotationMatrix(theta, 4, "r", axis)
    bMatrix = matrix_rotate(axis, theta)

  else:
    if DotVecs(target, nor) > 0.0: updown =  1.0
    else:                          updown = -1.0

    # Quoted from Blender source : "I think this should work ..."
    bMatrix = Matrix([updown, 0.0, 0.0, 0.0],
      		     [0.0, updown, 0.0, 0.0],
      		     [0.0, 0.0, 1.0, 0.0],
      		     [0.0, 0.0, 0.0, 1.0])

  #rMatrix = RotationMatrix(roll, 4, "r", nor)
  rMatrix = matrix_rotate(nor, roll)
  return bMatrix * rMatrix

def rot_vertex(mm, v):
	lx,ly,lz=v[0],v[1],v[2]
	gx=(mm[0][0]*lx + mm[1][0]*ly + mm[2][0]*lz) + mm[3][0]
	gy=((mm[0][1]*lx + mm[1][1]*ly + mm[2][1]*lz) + mm[3][1])
	gz=((mm[0][2]*lx + mm[1][2]*ly+ mm[2][2]*lz) + mm[3][2])
	rotatedv=[gx,gy,gz]
	return rotatedv
#enddef

def add_to_list(uvs, uv):
	size = len(uvs)
	for i in range(size):
		if(uvs[i] == uv):
			return i
	uvs.append(uv)
	return size
#enddef


class MTWLine:
	def __init__(self, tag):
		self.tag = tag
		self.size = 0
		self.data = ""
	#enddef

	def to_string(self):
		return self.tag+pack('i', self.size)+self.data
	#enddef
#endclass



class Skeleton:
  def __init__(self, matrix):
    self.matrix = matrix
    self.bones = []
    self.next_bone_id = 0


BONES = {}
rootBones = []

class Bone:
  def __init__(self, skeleton, parent, b, loc, rot):
    self.parent = parent
    self.name   = b.getName()
    self.loc = Vector(loc)
    self.rot = rot.toMatrix()
    self.rot.resize4x4()
    self.children = []
    self.b = b

    self.matrix = self.rot * TranslationMatrix(self.loc)
    self.matrixNoParent = self.matrix
    if parent:
      self.matrix = self.matrix * parent.matrix
      parent.children.append(self)
    self.oMatrix = self.matrix
    
    self.skeleton = skeleton
    self.id = skeleton.next_bone_id
    skeleton.next_bone_id += 1
    skeleton.bones.append(self)

    BONES[self.name] = self


ANIMATIONS = {}
animationsNames = []

class Animation:
  def __init__(self, name, duration = 0.0):
    self.name     = name
    self.duration = duration
    self.tracks   = {} # Map bone names to tracks
    self.keyFramesTime = []

class Track:
  def __init__(self, animation, bone):
    self.bone      = bone
    self.keyframes = []
    self.animation = animation
    animation.tracks[bone.name] = self


class KeyFrame:
  def __init__(self, track, time, loc, rot):
    self.time = time
    self.matrix = rot.toMatrix()
    self.matrix.resize4x4()
    self.matrix = self.matrix * TranslationMatrix(Vector([loc[0], loc[1], loc[2]]))
    self.track = track
    track.keyframes.append(self)




def sort_bones():
	#match there ID with there place in the list
	cpt = 0
	for b in BONES:
		BONES[b].id = cpt
		cpt +=1


def write_object(shape):
	ret = MTWLine("Objt")
	name = shape.getName()

	print "Exporting %s..." % name

	ret.data += "NAME"+pack('i', (len(name) + 1))+name+"\0"

	ret.size += len(name) + 9 #longueur totale, TAG et taille compris

	takeBones = (len(BONES) != 0)
	if takeBones:
		skel = MTWLine("RPOS")

		for b in BONES: #to ensure we go through the list in the bone ID order
			loc = BONES[b].oMatrix.translationPart()
			rot = BONES[b].oMatrix.toQuat()
			print "%s (id = %s): %s %s\n" % (b, BONES[b].id, loc, rot)
			skel.data += pack('fffffff', loc[0], -loc[1], loc[2],
						 -rot[1], rot[2], -rot[3], rot[0])
			skel.size += 28

		ret.data += skel.to_string()
		ret.size += skel.size + 8

		mSkel = MTWLine("SKEL")
		nbInfluences = 0

	vertices = shape.getData().verts
	nbVertices = len(vertices)
	mVect = MTWLine("VECT")
	mVect.size = nbVertices * 12
	for vertex in vertices:
		if takeBones:
			influences = shape.getData().getVertexInfluences(vertex.index)
			if not influences: print "Warning !  A vertex has no influence !"#remplacer par un popup
			# sum of influences is not always 1.0 in Blender ?!?!
			sum = 0.0
			for bone_name, weight in influences: sum += weight

			for bone_name, weight in influences:
				if sum != 0:
					nbInfluences += 1
					mSkel.data += pack('HHf', vertex.index, BONES[bone_name].id, weight / sum)

		v = rot_vertex(shape.getMatrix(), vertex)
		mVect.data += pack('fff', v[0],-v[1],v[2])


	ret.data += mVect.to_string()

	ret.size += mVect.size + 8
	

	if takeBones:
		mSkel.size = nbInfluences * 8
		ret.data += mSkel.to_string()
		ret.size += mSkel.size + 8

	takeText = shape.getData().hasFaceUV()

	textName = ""
	mTextIdx = MTWLine("MID0")
	mTextUVList = [[0, 0]]

	faces = shape.getData().faces
	mIndx = MTWLine("INDX")
	for face in faces:
		fsize = len(face)
		if(fsize > 4):
			print "bad face: too much indexes"
		elif(fsize < 3):
			print "bad face: not enought indexes"
		elif(fsize == 3):
			mIndx.data += pack('HHH', face[2].index, face[1].index, face[0].index)
			mIndx.size += 3
			if(takeText):
				if(face.image):
					if(textName == ""):
						textName = face.image.name
					if(textName == face.image.name):
						mTextIdx.data += pack('HHH', add_to_list(mTextUVList, face.uv[2]),
									add_to_list(mTextUVList, face.uv[1]),
									add_to_list(mTextUVList, face.uv[0]))
					else:
						print "Error: multi Texturing on face"
						mTextIdx.data += pack('HHH', 0, 0, 0)
				else:
					mTextIdx.data += pack('HHH', 0, 0, 0)

		else:
			mIndx.data += pack('HHH', face[2].index, face[1].index, face[0].index)
			mIndx.data += pack('HHH', face[3].index, face[2].index, face[0].index)
			mIndx.size += 6
			if(takeText):
				if(face.image):
					if(textName == ""):
						textName = face.image.name
					if(textName == face.image.name):
						mTextIdx.data += pack('HHH', add_to_list(mTextUVList, face.uv[2]),
									add_to_list(mTextUVList, face.uv[1]),
									add_to_list(mTextUVList, face.uv[0]))
						mTextIdx.data += pack('HHH', add_to_list(mTextUVList, face.uv[3]),
									add_to_list(mTextUVList, face.uv[2]),
									add_to_list(mTextUVList, face.uv[0]))
					else:
						print "Error: multi Texturing on face"
						mTextIdx.data += pack('HHHHHH', 0, 0, 0, 0, 0, 0)
				else:
					mTextIdx.data += pack('HHHHHH', 0, 0, 0, 0, 0, 0)

  #endfor


	mIndx.size *= 2
	mTextIdx.size = mIndx.size
	ret.data += mIndx.to_string()
	ret.size += mIndx.size + 8


	if(takeText and textName != ""):
		print "Textured with %s" % textName
		ret.size += len(textName) + 9
		ret.data += "TEX0"+pack('i', len(textName) + 1)+textName+"\0"

		mTextUV = MTWLine("MAP0")
		mTextUV.size = len(mTextUVList) * 8
		for u in mTextUVList:
			mTextUV.data += pack('ff', u[0], 1.0 - u[1])

		ret.size += mTextUV.size + 8 + mTextIdx.size + 8
		ret.data += mTextUV.to_string() + mTextIdx.to_string()

	return ret
#enddef

def root(boneList):
	r = []
	for b in boneList:
		if not b.hasParent():
			r.append(b)
	return r
			
def update():
	global animationMenuName
	animationMenuName = " Animations:%t"
	saveFrame = scene.currentFrame()

	for obj in Blender.Object.Get():
		data = obj.getData()
		if type(data) is Blender.Types.ArmatureType:
			matrix = obj.getMatrix()
			skeleton = Skeleton(matrix)
			def treat_bone(b, parent = None):
				head = b.getHead()
				tail = b.getTail()
				head = Vector([head[0], head[1], head[2], 1])
				tail = Vector([tail[0], tail[1], tail[2], 1])
				
				# Turns the Blender's head-tail-roll notation into a quaternion
				temp = blender_bone2matrix(head, tail, b.getRoll())
				quat = temp.toQuat()

				if parent:
					# Compute the translation from the parent bone's head to the child
					# bone's head, in the parent bone coordinate system.
					# The translation is parent_tail - parent_head + child_head,
					# but parent_tail and parent_head must be converted from the parent's parent
					# system coordinate into the parent system coordinate.

					parent_invert_transform = CopyMat(parent.rot)
					parent_invert_transform.invert()
					parent_head = VecMultMat(parent.head, parent_invert_transform)
					parent_tail = VecMultMat(parent.tail, parent_invert_transform)

					bone = Bone(skeleton, parent, b, [parent_tail[0] - parent_head[0] + head[0], parent_tail[1] - parent_head[1] + head[1], parent_tail[2] - parent_head[2] + head[2]], quat)
				else:
					# Apply the armature's matrix to the root bones
					head = VecMultMat(head, matrix)
					tail = VecMultMat(tail, matrix)
					quat = (temp * matrix).toQuat()

					# Here, the translation is simply the head vector
					bone = Bone(skeleton, parent, b, [head[0], head[1], head[2]], quat)

					rootBones.append(b)

				bone.head = head
				bone.tail = tail

				for child in b.getChildren(): treat_bone(child, bone)

			for b in root(data.getBones()): treat_bone(b)

			# Only one armature / skeleton
			break
  	sort_bones()

	if(len(BONES) == 0):
		return;

	for ipo in Blender.Ipo.Get():
		name = ipo.getName()
		if not name:
			continue
		splitted = cut_name(name)
		if(not splitted):
			print "invalid IPO Curve name %s...skipping" % name
			continue

		animation_name, bone_name = splitted
		animation = ANIMATIONS.get(animation_name)
		if not animation:
			animation = ANIMATIONS[animation_name] = Animation(animation_name)
			animationMenuName += "|" + animation_name
			animationsNames.append(animation_name)

		try: bone  = BONES[bone_name]
		except KeyError:
			print "invalid bone %s...skipping" % bone_name
			continue

		track = animation.tracks.get(bone_name)
		if not track:
			track = animation.tracks[bone_name] = Track(animation, bone)
			track.finished = 0

		try: nb_bez_pts = ipo.getNBezPoints(0)
		except TypeError:
			print "No key frame for animation %s, bone %s, skipping..." % (animation_name, bone_name)
			nb_bez_pts = 0

		for bez in range(nb_bez_pts): 
			add_to_list(animation.keyFramesTime, ipo.getCurveBeztriple(0, bez)[3])

		animation.duration = len(animation.keyFramesTime)

	animationMenuName += "|All"

	for ipo in Blender.Ipo.Get():
		name = ipo.getName()
		if not name:
			continue
		splitted = cut_name(name)
		if(not splitted):
			print "invalid IPO Curve name %s...skipping" % name
			continue

		animation_name, bone_name = splitted
		animation = ANIMATIONS.get(animation_name)
		try: bone  = BONES[bone_name]
		except KeyError:
			print "invalid bone %s...skipping" % bone_name
			continue
		track = animation.tracks.get(bone_name)

		animation.keyFramesTime.sort()
		for time in animation.keyFramesTime:
			scene.currentFrame(int(time))

			# Needed to update IPO's value, but probably not the best way for that...
			sc.makeCurrent()
			sc.update(1)
			KeyFrame(track, time, bone.b.getLoc(), bone.b.getQuat())



	scene.currentFrame(saveFrame)

	# Needed to update IPO's value, but probably not the best way for that...
	sc.makeCurrent()
	sc.update(1)


def create_mesh_file(filename):
	file = open(filename, "wb")
	#export skeleton

	header = MTWLine("MTW2")

	count = 0
	shapes = Blender.Object.Get()
	for shape in shapes:
		if(shape.getType() == "Mesh"):
			t = write_object(shape)
			header.size = header.size + t.size + 8
			header.data = header.data + t.to_string()
			count += 1
		#endif
	#endfor

	file.write(header.to_string())
	file.close()

	print "\n%s object(s) exported in %s" % (count, filename)


def write_keyFrame(animation, keyFrame):
	ret = MTWLine("Fram")
	kfnb = -1

	def treat_bone(b, parent = None):
		bone = BONES[b.getName()]
		try: t  = animation.tracks[b.getName()]
		except KeyError:
			print "bone %s not use in this animation...skipping" % b.getName()
			return
		trk = t.keyframes[keyFrame]
		modifMatrix = trk.matrix
		originalMatrix = bone.matrixNoParent
		if parent:
			originalMatrix = originalMatrix * parent.matrix
		bone.matrix = modifMatrix * originalMatrix
      		for child in b.getChildren(): treat_bone(child, bone)

	for b in rootBones:
		treat_bone(b)

	skel = MTWLine("DATA")

	for b in BONES: #to ensure we go through the list in the bone ID order
		try: t  = animation.tracks[b]
		except KeyError:
			print "bone %s not use in this animation...skipping" % b
			#prevent problem indexing bones
			skel.data += pack('fffffff', 0, 0, 0, 0, 0, 0, 0)
			skel.size += 28
			continue
		trk = t.keyframes[keyFrame]

		if (kfnb < 0): kfnb = trk.time
		loc = BONES[b].matrix.translationPart()
		rot = BONES[b].matrix.toQuat()
		print "%s (id = %s): %s %s\n" % (b, BONES[b].id, loc, rot)
		skel.data += pack('fffffff', loc[0], -loc[1], loc[2],
					 -rot[1], rot[2], -rot[3], rot[0])
		skel.size += 28
	print "frame %s\n\n" % kfnb
	ret.data += "frnb"+pack('i', kfnb - 1)
	ret.size += 8

	ret.data += skel.to_string()
	ret.size += skel.size + 8

	return ret


def write_anim(animName):
	ret = MTWLine("Anim")
	ret.data += "sKEL"
	ret.size += 4
	ret.data += "NAME"+pack('i', (len(animName) + 1))+animName+"\0"

	ret.size += len(animName) + 9 #longueur totale, TAG et taille compris


	ret.data += "nfrm"+pack('i', scene.endFrame())
	ret.size += 8

	animation = ANIMATIONS.get(animName)
	nbKeyFrames = animation.duration

	for keyFrame in range(nbKeyFrames):
		t = write_keyFrame(animation, keyFrame)
		ret.data += t.to_string()
		ret.size += t.size + 8

	return ret


def cut_name(name):
	# Try to extract the animation name and the bone name from the IPO name.
	# THIS MAY NOT WORK !!!
	# The animation name extracted here is usually NOT the name of the action in Blender

	splitted = name.split(".")
	if len(splitted) == 2:
		animation_name, bone_name = splitted
	elif len(splitted) == 3:
		animation_name, a, b = splitted
		if   a[0] in string.digits:
			animation_name += "." + a
			bone_name = b
		elif b[0] in string.digits:
			bone_name = a + "." + b
		elif b[0] == 'L':
			bone_name = a + "." + b
		elif b[0] == 'R':
			bone_name = a + "." + b
		else:
			print "Un-analysable IPO name :", name
			return None
	elif len(splitted) == 4:
			animation_name, a, b, c = splitted
			if   a[0] in string.digits:
				animation_name += "." + a
				bone_name = b+"."+c
			elif c[0] == 'L':
				bone_name = b + "." + c
			elif c[0] == 'R':
				bone_name = b + "." + c
			else:
				print "Un-analysable IPO name :", name
			return None
	else:
		print "Un-analysable IPO name :", name
		return None
	return [animation_name, bone_name]


def create_animation_file(filename):
	file = open(filename, "wb")
	header = MTWLine("MTW2")

	if animationMenu.val > len(animationsNames):
		for a in ANIMATIONS:
			t = write_anim(a)
			header.data += t.to_string()
			header.size += t.size + 8
	else:
		a = animationsNames[animationMenu.val - 1]
		t = write_anim(a)
		header.data += t.to_string()
		header.size += t.size + 8

	file.write(header.to_string())
	file.close()


def mtw_file_callback(filename):
	if filename.find('.mtw', -4) < 0: filename += '.mtw'
	create_mesh_file(filename)

def atw_file_callback(filename):
	if filename.find('.atw', -4) < 0: filename += '.atw'
	create_animation_file(filename)

#enddef

#--------------------------------------------------------------------
global animationMenu
global animationMenuName

def gui():
	global animationMenu
	global animationMenuName
	BGL.glClearColor(0,0,1,1)
	BGL.glClear(BGL.GL_COLOR_BUFFER_BIT)
	BGL.glColor3f(1,1,1)
	Draw.Button("Export Mesh", 10, 10, 100, 140, 20, "Export mesh as mtw file")
	Draw.Button("Exit", 11, 205, 10, 55, 20, "Exit script")
	Draw.Button("Update", 13, 10, 10, 55, 20, "Update scipt data if you have change scene")
	animationMenu = Draw.Menu(animationMenuName, 12, 10, 70, 160, 20, len(animationsNames) + 1, "Choose animation to export.")
	Draw.Button("Export Animation(s)", 14, 180, 70, 140, 20, "Export animation(s) as atw file")

def event(evt, val):
	print

def button_event(evt):
	global animationMenu, ANIMATIONS, BONES, scene, sc, rootBones, animationsNames
	if evt == 10:
		Window.FileSelector(mtw_file_callback,"Save MTW")
	elif evt == 11:
		print "End *TW export\n"
		Draw.Exit()
	elif evt == 12:
		print animationMenu.val
	elif evt == 13:
		#scene = Scene.getCurrent()
		scene = Scene.getCurrent().getRenderingContext()
		sc = Scene.getCurrent()
		BONES = {}
		rootBones = []
		ANIMATIONS = {}
		animationsNames = []
		update()
	elif evt == 14:
		if(len(ANIMATIONS)):
			Window.FileSelector(atw_file_callback,"Save ATW")

print "\nBegin *TW export"

#scene = Scene.getCurrent()
scene = Scene.getCurrent().getRenderingContext()
sc = Scene.getCurrent()
update()
Draw.Register(gui, None, button_event)  # registering the 3 callbacks




